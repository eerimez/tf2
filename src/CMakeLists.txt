cmake_minimum_required(VERSION 3.1)

project(tf2 CXX)

# C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#target_include_directories(tf2  . ../include)
include_directories( . ../include ../3rdparty/lz4/lib ../3rdparty/mongo-driver/src/mongoc ../3rdparty/mongo-driver/src/libbson/src/bson -isystem /usr/include/qt)
include_directories(-I. -I../include -I../3rdparty/lz4/lib -I../3rdparty/mongo-driver/src/mongoc -I../3rdparty/mongo-driver/src/libbson/src/bson -isystem /usr/include/qt
        -isystem /usr/include/qt/QtSql
        -isystem /usr/include/qt/QtQml
        -isystem /usr/include/qt/QtNetwork
        -isystem /usr/include/qt/QtXml
        -isystem /usr/include/qt/QtCore
        -I.obj
        -I/usr/lib/qt/mkspecs/linux-g++)

add_library(tf2
        twebapplication.cpp
        tpkcs12.cpp
        twritefilethread.cpp
tapplicationserverbase.cpp 
tthreadapplicationserver.cpp 
tactioncontext.cpp 
tdatabasecontext.cpp 
tactionthread.cpp 
thttpsocket.cpp 
tsendbuffer.cpp 
tabstractcontroller.cpp 
tactioncontroller.cpp 
directcontroller.cpp 
tactionview.cpp 
tactionmailer.cpp 
tsqldatabase.cpp 
tsqldatabasepool.cpp 
tsqlobject.cpp 
tsqlormapperiterator.cpp 
tsqlquery.cpp 
tsqlqueryormapper.cpp 
tsqlqueryormapperiterator.cpp 
tsqltransaction.cpp 
tsqldriverextension.cpp 
tsqldriverextensionfactory.cpp 
tcriteria.cpp 
tcriteriaconverter.cpp 
thttprequest.cpp 
thttpresponse.cpp 
tmultipartformdata.cpp 
tcontentheader.cpp 
thttputility.cpp 
thtmlattribute.cpp 
ttextview.cpp 
tdirectview.cpp 
tactionhelper.cpp 
tviewhelper.cpp 
tprototypeajaxhelper.cpp 
toption.cpp 
ttemporaryfile.cpp 
tcookiejar.cpp 
tsession.cpp 
tsessionmanager.cpp 
tsessionstore.cpp 
tsessionstorefactory.cpp 
tsessionsqlobjectstore.cpp 
tsessionmongostore.cpp 
tsessioncookiestore.cpp 
tsessionfilestore.cpp 
tsessionfiledbstore.cpp 
tsessionredisstore.cpp 
thtmlparser.cpp 
tabstractmodel.cpp 
tmodelobject.cpp 
tsystemglobal.cpp 
tglobal.cpp 
taccesslog.cpp 
taccesslogstream.cpp 
tlog.cpp 
tlogger.cpp 
tloggerfactory.cpp 
tfilelogger.cpp 
tabstractlogstream.cpp 
tsharedmemorylogstream.cpp 
tbasiclogstream.cpp 
tmailmessage.cpp 
tsmtpmailer.cpp 
tpopmailer.cpp 
tsendmailmailer.cpp 
tcryptmac.cpp 
tinternetmessageheader.cpp 
thttpheader.cpp 
turlroute.cpp 
tabstractuser.cpp 
tformvalidator.cpp 
taccessvalidator.cpp 
tpaginator.cpp 
tkvsdatabase.cpp 
tkvsdatabasepool.cpp 
tkvsdriver.cpp 
tredisdriver.cpp 
tredis.cpp 
tfileaiologger.cpp 
tfileaiowriter.cpp 
tscheduler.cpp 
tapplicationscheduler.cpp 
tappsettings.cpp 
tabstractwebsocket.cpp 
twebsocket.cpp 
twebsocketendpoint.cpp 
twebsocketframe.cpp 
twebsocketworker.cpp 
twebsocketsession.cpp 
tpublisher.cpp 
tsystembus.cpp 
tprocessinfo.cpp 
tbasictimer.cpp 
tatomicptr.cpp 
thazardptr.cpp 
thazardobject.cpp 
thazardptrmanager.cpp 
tatomic.cpp 
tstack.cpp 
tqueue.cpp 
tdatabasecontextthread.cpp 
tdatabasecontextmainthread.cpp 
tbackgroundprocess.cpp 
tbackgroundprocesshandler.cpp 
tdebug.cpp 
tjsonutil.cpp 
tjsloader.cpp 
tjsmodule.cpp 
tjsinstance.cpp 
treactcomponent.cpp 
tcache.cpp 
tcachefactory.cpp 
tcachestore.cpp 
tcachesqlitestore.cpp 
tcachemongostore.cpp 
tcacheredisstore.cpp 
tactioncontroller_qt5.cpp 
twebapplication_unix.cpp 
tapplicationserverbase_unix.cpp 
tfileaiowriter_unix.cpp 
tredisdriver_unix.cpp 
tmultiplexingserver_linux.cpp 
tactionworker.cpp 
tepoll.cpp 
tepollsocket.cpp 
tepollhttpsocket.cpp 
tepollwebsocket.cpp 
tprocessinfo_linux.cpp 
tthreadapplicationserver_linux.cpp 
tmongodriver.cpp 
tmongoquery.cpp 
tmongocursor.cpp 
tbson.cpp 
tmongoobject.cpp 
tmongoodmapper.cpp 
tcriteriamongoconverter.cpp )
