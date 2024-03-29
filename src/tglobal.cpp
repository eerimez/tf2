/* Copyright (c) 2010-2019, AOYAMA Kazuharu
 * All rights reserved.
 *
 * This software may be used and distributed according to the terms of
 * the New BSD License, which is incorporated herein by reference.
 */

#include <TGlobal>
#include <TWebApplication>
#include <TAppSettings>
#include <TActionContext>
#include <TDatabaseContext>
#include <TActionThread>
#include <TCache>
#include "tdatabasecontextthread.h"
#include <QDataStream>
#include <QBuffer>
#include "lz4.h"
#ifdef Q_OS_LINUX
# include <TActionWorker>
#endif
#ifdef Q_OS_WIN
# include <Windows.h>
#endif
#include <cstdlib>
#include <climits>
#include <random>

constexpr int LZ4_BLOCKSIZE = 1024 * 1024; // 1 MB

/*!
  Returns a global pointer referring to the unique application object.
*/
TWebApplication *Tf::app()
{
    return static_cast<TWebApplication *>(qApp);
}

/*!
  Returns a global pointer referring to the unique application settings object.
*/
TAppSettings *Tf::appSettings()
{
    return TAppSettings::instance();
}

/*!
  Returns the map associated with config file \a configName in 'conf'
  directory.
*/
const QVariantMap &Tf::conf(const QString &configName)
{
    return Tf::app()->getConfig(configName);
}

/*!
  Causes the current thread to sleep for \a msecs milliseconds.
*/
void Tf::msleep(unsigned long msecs)
{
    QThread::msleep(msecs);
}

/*
  Xorshift random number generator implement
*/
namespace {
    QMutex randMutex;
    quint32 x = 123456789;
    quint32 y = 362436069;
    quint32 z = 987654321;
    quint32 w = 1;
}

/*!
  Sets the argument \a seed to be used to generate a new random number sequence
  of xorshift random integers to be returned by randXor128().
  This function is thread-safe.
*/
void Tf::srandXor128(quint32 seed)
{
    randMutex.lock();
    w = seed;
    z = w ^ (w >> 8) ^ (w << 5);
    randMutex.unlock();
}

/*!
  Returns a value between 0 and UINT_MAX, the next number in the current
  sequence of xorshift random integers.
  This function is thread-safe.
*/
quint32 Tf::randXor128()
{
    QMutexLocker lock(&randMutex);
    quint32 t;
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return w;
}

namespace {
    std::random_device randev;
    std::mt19937     mt(randev());
    QMutex           mtmtx;
    std::mt19937_64  mt64(randev());
    QMutex           mt64mtx;
}

uint32_t Tf::rand32_r()
{
    mtmtx.lock();
    uint32_t ret = mt();
    mtmtx.unlock();
    return ret;
}


uint64_t Tf::rand64_r()
{
    mt64mtx.lock();
    uint64_t ret = mt64();
    mt64mtx.unlock();
    return ret;
}

/*!
  Random number generator in the range from \a min to \a max.
*/
uint64_t Tf::random(uint64_t min, uint64_t max)
{
    std::uniform_int_distribution<uint64_t> uniform(min, max);
    mt64mtx.lock();
    uint64_t ret = uniform(mt64);
    mt64mtx.unlock();
    return ret;
}

/*!
  Random number generator in the range from 0 to \a max.
*/
uint64_t Tf::random(uint64_t max)
{
    return random(0, max);
}


TCache *Tf::cache()
{
    return Tf::currentContext()->cache();
}


TActionContext *Tf::currentContext()
{
    TActionContext *context = nullptr;

    switch ( Tf::app()->multiProcessingModule() ) {
    case TWebApplication::Thread:
        context = qobject_cast<TActionThread *>(QThread::currentThread());
        if (Q_LIKELY(context)) {
            return context;
        }
        break;

    case TWebApplication::Epoll:
#ifdef Q_OS_LINUX
        return TActionWorker::instance();
#else
        tFatal("Unsupported MPM: epoll");
#endif
        break;

    default:
        break;
    }

    throw RuntimeException("Can not cast the current thread", __FILE__, __LINE__);
}


TDatabaseContext *Tf::currentDatabaseContext()
{
    TDatabaseContext *context;

    context = TDatabaseContext::currentDatabaseContext();
    if (context) {
        return context;
    }

    context = dynamic_cast<TDatabaseContext*>(QThread::currentThread());
    if (context) {
        return context;
    }

    throw RuntimeException("Can not cast the current thread", __FILE__, __LINE__);
}


QSqlDatabase &Tf::currentSqlDatabase(int id)
{
    return currentDatabaseContext()->getSqlDatabase(id);
}


QMap<QByteArray, std::function<QObject*()>> *Tf::objectFactories()
{
    static QMap<QByteArray, std::function<QObject*()>> objectFactoryMap;
    return &objectFactoryMap;
}


QByteArray Tf::lz4Compress(const char *data, int nbytes, int compressionLevel)
{
    // internal compress function
    auto compress = [](const char *src, int srclen, int level, QByteArray &buffer) {
        const int bufsize = LZ4_compressBound(srclen);
        buffer.reserve(bufsize);

        if (srclen > 0) {
            int rv = LZ4_compress_fast(src, buffer.data(), srclen, bufsize, level);
            if (rv > 0) {
                buffer.resize(rv);
            } else {
                tError("LZ4 compression error: %d", rv);
                buffer.clear();
            }
        } else {
            buffer.resize(0);
        }
    };

    QByteArray ret;
    int rsvsize = LZ4_compressBound(nbytes);
    if (rsvsize < 1) {
        return ret;
    }

    ret.reserve(rsvsize);
    QDataStream dsout(&ret, QIODevice::WriteOnly);
    dsout.setByteOrder(QDataStream::LittleEndian);
    QByteArray buffer;
    int readlen = 0;

    while (readlen < nbytes) {
        int datalen = qMin(nbytes - readlen, LZ4_BLOCKSIZE);
        compress(data + readlen, datalen, compressionLevel, buffer);
        readlen += datalen;

        if (buffer.isEmpty()) {
            ret.clear();
            break;
        } else {
            dsout << (int)buffer.length();
            dsout.writeRawData(buffer.data(), buffer.length());
        }
    }

    return ret;
}


QByteArray Tf::lz4Compress(const QByteArray &data, int compressionLevel)
{
    return Tf::lz4Compress(data.data(), data.length(), compressionLevel);
}


QByteArray Tf::lz4Uncompress(const char *data, int nbytes)
{
    QByteArray ret;
    QBuffer srcbuf;
    const int CompressBoundSize = LZ4_compressBound(LZ4_BLOCKSIZE);

    srcbuf.setData(data, nbytes);
    srcbuf.open(QIODevice::ReadOnly);
    QDataStream dsin(&srcbuf);
    dsin.setByteOrder(QDataStream::LittleEndian);

    QByteArray buffer;
    buffer.reserve(LZ4_BLOCKSIZE);

    int readlen = 0;
    while (readlen < nbytes) {
        int srclen;
        dsin >> srclen;
        readlen += sizeof(srclen);

        if (srclen <= 0 || srclen > CompressBoundSize) {
            tError("LZ4 uncompression format error");
            ret.clear();
            break;
        }

        int rv = LZ4_decompress_safe(data + readlen, buffer.data(), srclen, LZ4_BLOCKSIZE);
        dsin.skipRawData(srclen);
        readlen += srclen;

        if (rv > 0) {
            buffer.resize(rv);
            ret += buffer;
        } else {
            tError("LZ4 uncompression error: %d", rv);
            ret.clear();
            break;
        }
    }
    return ret;
}


QByteArray Tf::lz4Uncompress(const QByteArray &data)
{
    return Tf::lz4Uncompress(data.data(), data.length());
}


/*!
  \def T_EXPORT(VAR)
  Exports the current value of a local variable named \a VAR from the
  controller context to view contexts.
  \see T_FETCH(TYPE,VAR)
 */

/*!
  \def texport(VAR)
  Exports the current value of a local variable named \a VAR from the
  controller context to view contexts.
  \see tfetch(TYPE,VAR)
 */

/*!
  \def T_EXPORT_UNLESS(VAR)
  Exports the current value of a local variable named \a VAR from the
  controller context to view contexts only if a local variable named
  \a VAR isn't exported.
  \see T_EXPORT(VAR)
 */

/*!
  \def texportUnless(VAR)
  Exports the current value of a local variable named \a VAR from the
  controller context to view contexts only if a local variable named
  \a VAR isn't exported.
  \see texport(VAR)
 */

/*!
  \def T_FETCH(TYPE,VAR)
  Creates a local variable named \a VAR with the type \a TYPE on the view and
  fetches the value of a variable named \a VAR exported in a controller context.
  \see T_EXPORT(VAR)
 */

/*!
  \def tfetch(TYPE,VAR)
  Creates a local variable named \a VAR with the type \a TYPE on the view and
  fetches the value of a variable named \a VAR exported in a controller context.
  \see texport(VAR)
 */
