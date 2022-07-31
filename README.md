# GooseBt

Developing GooseBt Anti-virus Framework

Scheduled -> Developing -> Testing -> Release

## DriverLoader.exe

Testing

A console application to install and start GooseBt drivers. All functions are not stable still. Administrator permission is required. Commandline mode is supported. A tool written by others would be started if no valid arguments in commandline. 

## Scanner.exe

Release

A console application to simply scan file on the disk, merely a second-generation anti-virus software scanner, but without any database. Currently, it could only trigger scaning by third-party antivirus engines. Comparing a specified file with an known virus file is supported. 

## SVCLauncher.exe

Testing

A console application to install and start GooseBt service. The creation and starting are not stable still. Administrator permission is required. Only commandline mode is supported. Both commandline mode and interactive mode are supported. 

## wmif.exe

Release

A console application to manage file. Only commandline mode is supported. 

## wmip.exe

Release

A console application to manage process. Both commandline mode and interactive mode are supported. 

## ZwDeleteFile

Testing

A kernel driver to delete file forcibly in kernel mode. Currently, it could not remove occuried or protected files still. Forcibly removing directory is not supported. 

## ZwOpReg

Scheduled

A kernel driver to operate the Registry in kernel mode. 

## ZwTerminateProcess.sys

Release

A kernel driver to terminate process by two kernel methods. 
