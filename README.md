# GooseBt

Developing GooseBt Anti-virus Framework

Scheduled -> Developing -> Testing -> Release

## Win DOS

This is a folder, which should be hidden. The folder provides script tools written in Batch and VBS language. When one is in Windows PE or extreme limitation mode including no drivers or services could be installed or started, the tools could do a favor. 

## AntiVirusMon.exe

Scheduled

A console application to test the loading of DLL file. Currently, it has nothing to do since GooseBtDll.dll has nothing to do. 

## ClickProgram.exe

Scheduled

A GUI written by C\#. This is used for automatic clicking and Bad HID testing. 

## DriverLoader.exe

Testing

A console application to install and start GooseBt drivers. All functions are not stable still. Administrator permission is required. Commandline mode is supported. A tool written by others would be started if no valid arguments in commandline. 

## GooseBtDll.dll

Scheduled

A DLL file to help run the dynamic codes in different Windows environment. Currently, it has nothing to do. 

## GooseBtMain.exe

Developing

An MFC application to provide the main window. 

## GooseBtMonitor.exe

Testing

A console application to check out whether GooseBt itself is running actively and normally. 

## GooseBtSVC.exe

Release

A service to help run the GooseBt. It would create a log in its directory. 

## GooseBtTray.exe

Developing

An MFC application to show the tray and action. 

## GooseBtUpdate.exe

Testing

A console application to update the whole GooseBt solution. However, it remains problems that replacing an opening files would be failed. 

## KMDFFile.sys

Scheduled

A kernel driver to monitor files' action. 

## KMDFFileProtector.sys

Testing

A kernel driver to protect files from being deleted. Currently, Just merely open targets in kernel, which makes it impossible to read, write, execute targets as well. 

## KMDFKeyboard.sys

Scheduled

A kernel driver to monitor keyboard. 

## KMDFProcessProtector.sys

A kernel driver to protect processes. Note that ZwTerminateProcess.sys can defeat this protection. 

## KMDFReg.sys

Scheduled

A kernel driver to monitor Registry. 

## KMDFSerial.sys

Release

A kernel driver to monitor data flows among serials. 

## KMDFThread.sys

Release

A kernel driver to monitor the creation and deletion of processes and threads. 

## NTNoFileCreate.sys

Developing

A kernel driver to prevent files from being created. 

## NTNoProcessCreate.sys

Release

A kernel driver to forbid creating a new process. Module loading prevention is also included. 

## Scanner.exe

Release

A console application to simply scan file on the disk, merely a second-generation anti-virus software scanner, but without any database. Currently, it could only trigger scaning by third-party antivirus engines. Comparing a specified file with an known virus file is supported. Both commandline mode and interactive mode are supported. 

## SVCLauncher.exe

Testing

A console application to install and start GooseBt service. The creation and starting are not stable still. Administrator permission is required. Only commandline mode is supported. Only commandline mode is supported. 

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

A kernel driver to terminate process by two kernel methods. Terminating anti-virus software processes that own self-protection is available on x64. Termination on x86 has not been tested. For x86, Micropoint Proactive Defense Software and PC Hunter Standard 32 are strongly recommended. 
