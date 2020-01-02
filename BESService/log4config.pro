#配置文件（其它日志级别配置相同）：
log4cplus.rootLogger=TRACE, ALL_MSGS , DEBUG_MSGS, ERROR_MSGS, WARN_MSGS ,CONSOLE
log4cplus.appender.ALL_MSGS=log4cplus::TimeBasedRollingFileAppender
log4cplus.appender.ALL_MSGS.Append=true
log4cplus.appender.ALL_MSGS.Schedule=DAILY
log4cplus.appender.ALL_MSGS.DatePattern = yyyy-MM-dd
#log4cplus.appender.ALL_MSGS.File=log/AllMsg.log
log4cplus.appender.ALL_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/AllMsg.log
log4cplus.appender.ALL_MSGS.MaxFileSize=10KB
log4cplus.appender.ALL_MSGS.CreateDirs=true
log4cplus.appender.ALL_MSGS.MaxBackupIndex=10
log4cplus.appender.ALL_MSGS.layout=log4cplus::PatternLayout
log4cplus.appender.ALL_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n

#DEBUG
#设置日志追加到文件尾
log4cplus.appender.DEBUG_MSGS=log4cplus::TimeBasedRollingFileAppender


#设置每天记录一个日志文件
log4cplus.appender.DEBUG_MSGS.Schedule=DAILY
log4cplus.appender.DEBUG_MSGS.DatePattern=yyyy-MM-dd


#设置日志文件大小
log4cplus.appender.DEBUG_MSGS.MaxFileSize=100MB


#设置生成日志最大个数
log4cplus.appender.DEBUG_MSGS.MaxBackupIndex=10


#设置输出日志路径
#log4cplus.appender.DEBUG_MSGS.File=log/debug.log
log4cplus.appender.DEBUG_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/debug.log
log4cplus.appender.DEBUG_MSGS.CreateDirs=true
log4cplus.appender.DEBUG_MSGS.layout=log4cplus::PatternLayout


#设置日志打印格式
log4cplus.appender.DEBUG_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n


#匹配相同日志级别，只有debug日志才输入到该文件中
log4cplus.appender.DEBUG_MSGS.filters.1=log4cplus::spi::LogLevelMatchFilter
log4cplus.appender.DEBUG_MSGS.filters.1.LogLevelToMatch=DEBUG
log4cplus.appender.DEBUG_MSGS.filters.1.AcceptOnMatch=true
log4cplus.appender.DEBUG_MSGS.filters.2=log4cplus::spi::DenyAllFilter


#ERROR
#设置日志追加到文件尾
log4cplus.appender.ERROR_MSGS=log4cplus::TimeBasedRollingFileAppender
log4cplus.appender.ERROR_MSGS.Append=true


#设置每天记录一个日志文件
log4cplus.appender.ERROR_MSGS.Schedule=DAILY
log4cplus.appender.ERROR_MSGS.DatePattern=yyyy-MM-dd


#设置日志文件大小
log4cplus.appender.ERROR_MSGS.MaxFileSize=100MB


#设置生成日志最大个数
log4cplus.appender.ERROR_MSGS.MaxBackupIndex=10


#设置输出日志路径
#log4cplus.appender.ERROR_MSGS.File=log/error.log
log4cplus.appender.ERROR_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/error.log
log4cplus.appender.ERROR_MSGS.CreateDirs=true


#设置日志打印格式
log4cplus.appender.ERROR_MSGS.layout=log4cplus::PatternLayout
log4cplus.appender.ERROR_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n


#匹配相同日志级别，只有debug日志才输入到该文件中
log4cplus.appender.ERROR_MSGS.filters.1=log4cplus::spi::LogLevelMatchFilter
log4cplus.appender.ERROR_MSGS.filters.1.LogLevelToMatch=ERROR
log4cplus.appender.ERROR_MSGS.filters.1.AcceptOnMatch=true
log4cplus.appender.ERROR_MSGS.filters.2=log4cplus::spi::DenyAllFilter


#WARN
#设置日志追加到文件尾
log4cplus.appender.WARN_MSGS=log4cplus::TimeBasedRollingFileAppender
log4cplus.appender.WARN_MSGS.Append=true


#设置每天记录一个日志文件
log4cplus.appender.WARN_MSGS.Schedule=DAILY
log4cplus.appender.WARN_MSGS.DatePattern=yyyy-MM-dd



#设置日志文件大小
log4cplus.appender.WARN_MSGS.MaxFileSize=100MB


#设置生成日志最大个数
log4cplus.appender.WARN_MSGS.MaxBackupIndex=10


#设置输出日志路径
#log4cplus.appender.WARN_MSGS.File=log/warn.log
log4cplus.appender.WARN_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/warn.log
log4cplus.appender.WARN_MSGS.CreateDirs=true

#设置日志打印格式
log4cplus.appender.WARN_MSGS.layout=log4cplus::PatternLayout
log4cplus.appender.WARN_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n


#匹配相同日志级别，只有debug日志才输入到该文件中
log4cplus.appender.WARN_MSGS.filters.1=log4cplus::spi::LogLevelMatchFilter
log4cplus.appender.WARN_MSGS.filters.1.LogLevelToMatch=WARN
log4cplus.appender.WARN_MSGS.filters.1.AcceptOnMatch=true
log4cplus.appender.WARN_MSGS.filters.2=log4cplus::spi::DenyAllFilter

#屏幕打印输出
log4cplus.appender.CONSOLE=log4cplus::ConsoleAppender
log4cplus.appender.CONSOLE.filters.1.LogLevelToMatch=TRACE
log4cplus.appender.CONSOLE.filters.1.AcceptOnMatch=true
log4cplus.appender.CONSOLE.layout=log4cplus::PatternLayout
log4cplus.appender.CONSOLE.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n
