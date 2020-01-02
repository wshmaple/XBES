#�����ļ���������־����������ͬ����
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
#������־׷�ӵ��ļ�β
log4cplus.appender.DEBUG_MSGS=log4cplus::TimeBasedRollingFileAppender


#����ÿ���¼һ����־�ļ�
log4cplus.appender.DEBUG_MSGS.Schedule=DAILY
log4cplus.appender.DEBUG_MSGS.DatePattern=yyyy-MM-dd


#������־�ļ���С
log4cplus.appender.DEBUG_MSGS.MaxFileSize=100MB


#����������־������
log4cplus.appender.DEBUG_MSGS.MaxBackupIndex=10


#���������־·��
#log4cplus.appender.DEBUG_MSGS.File=log/debug.log
log4cplus.appender.DEBUG_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/debug.log
log4cplus.appender.DEBUG_MSGS.CreateDirs=true
log4cplus.appender.DEBUG_MSGS.layout=log4cplus::PatternLayout


#������־��ӡ��ʽ
log4cplus.appender.DEBUG_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n


#ƥ����ͬ��־����ֻ��debug��־�����뵽���ļ���
log4cplus.appender.DEBUG_MSGS.filters.1=log4cplus::spi::LogLevelMatchFilter
log4cplus.appender.DEBUG_MSGS.filters.1.LogLevelToMatch=DEBUG
log4cplus.appender.DEBUG_MSGS.filters.1.AcceptOnMatch=true
log4cplus.appender.DEBUG_MSGS.filters.2=log4cplus::spi::DenyAllFilter


#ERROR
#������־׷�ӵ��ļ�β
log4cplus.appender.ERROR_MSGS=log4cplus::TimeBasedRollingFileAppender
log4cplus.appender.ERROR_MSGS.Append=true


#����ÿ���¼һ����־�ļ�
log4cplus.appender.ERROR_MSGS.Schedule=DAILY
log4cplus.appender.ERROR_MSGS.DatePattern=yyyy-MM-dd


#������־�ļ���С
log4cplus.appender.ERROR_MSGS.MaxFileSize=100MB


#����������־������
log4cplus.appender.ERROR_MSGS.MaxBackupIndex=10


#���������־·��
#log4cplus.appender.ERROR_MSGS.File=log/error.log
log4cplus.appender.ERROR_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/error.log
log4cplus.appender.ERROR_MSGS.CreateDirs=true


#������־��ӡ��ʽ
log4cplus.appender.ERROR_MSGS.layout=log4cplus::PatternLayout
log4cplus.appender.ERROR_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n


#ƥ����ͬ��־����ֻ��debug��־�����뵽���ļ���
log4cplus.appender.ERROR_MSGS.filters.1=log4cplus::spi::LogLevelMatchFilter
log4cplus.appender.ERROR_MSGS.filters.1.LogLevelToMatch=ERROR
log4cplus.appender.ERROR_MSGS.filters.1.AcceptOnMatch=true
log4cplus.appender.ERROR_MSGS.filters.2=log4cplus::spi::DenyAllFilter


#WARN
#������־׷�ӵ��ļ�β
log4cplus.appender.WARN_MSGS=log4cplus::TimeBasedRollingFileAppender
log4cplus.appender.WARN_MSGS.Append=true


#����ÿ���¼һ����־�ļ�
log4cplus.appender.WARN_MSGS.Schedule=DAILY
log4cplus.appender.WARN_MSGS.DatePattern=yyyy-MM-dd



#������־�ļ���С
log4cplus.appender.WARN_MSGS.MaxFileSize=100MB


#����������־������
log4cplus.appender.WARN_MSGS.MaxBackupIndex=10


#���������־·��
#log4cplus.appender.WARN_MSGS.File=log/warn.log
log4cplus.appender.WARN_MSGS.FilenamePattern=log/%d{yyyy-MM-dd}/warn.log
log4cplus.appender.WARN_MSGS.CreateDirs=true

#������־��ӡ��ʽ
log4cplus.appender.WARN_MSGS.layout=log4cplus::PatternLayout
log4cplus.appender.WARN_MSGS.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n


#ƥ����ͬ��־����ֻ��debug��־�����뵽���ļ���
log4cplus.appender.WARN_MSGS.filters.1=log4cplus::spi::LogLevelMatchFilter
log4cplus.appender.WARN_MSGS.filters.1.LogLevelToMatch=WARN
log4cplus.appender.WARN_MSGS.filters.1.AcceptOnMatch=true
log4cplus.appender.WARN_MSGS.filters.2=log4cplus::spi::DenyAllFilter

#��Ļ��ӡ���
log4cplus.appender.CONSOLE=log4cplus::ConsoleAppender
log4cplus.appender.CONSOLE.filters.1.LogLevelToMatch=TRACE
log4cplus.appender.CONSOLE.filters.1.AcceptOnMatch=true
log4cplus.appender.CONSOLE.layout=log4cplus::PatternLayout
log4cplus.appender.CONSOLE.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S,%Q} %-4l [%t] %-5p %c{2} - %m%n
