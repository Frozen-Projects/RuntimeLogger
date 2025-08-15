# RuntimeLogger

## Description
This plugin allows you to store, write and visualize your runtime generated logs from various threads and create notifications about them. It is blueprint exposed. We did this plugin especially for shipping builds and runtime.

## Workflow
- <b>Log Manager (Blueprint Exposed)</b> is a UGameInstanceSubsystem (Runtime Logger Subsystem). So, it doesn't affected from level changes and has singleton/centralized design. So, you can access it from everywhere.</br>

- <b>Log Message (Blueprint Exposed)</b> function uses ``TQueue`` for gathering your messages from every thread without race condition. Each message is a ``json object``. You can define your own structure but it has automatic ``UUID (FGuid based)``, ``LogTime (FDateTime::Now())`` and ``LogLevel (ERuntimeLogLevels custom enum)`` parameters. We suggest you to add ``Plugin (or module) Name``, ``ClassName`` and ``FunctionName`` parameters. If you use ``C++`` you can use ``__FUNCSIG__`` to add class and function name automatically. It is a compiler feature.

- <b>Record Message</b> function is a public C++ function which running on FRunnableThread. So, it won't affect game thread performance even if there are lots of logs. This thread is on wait state if there is no log to record. So, other processes can use it and you won't waste your resources.</br>

It stores your log in a private ``TMap<FString, FString>`` which you can access with getters. Key is ``UUID`` and Value is ``other parameters than UUID``.</br>

It also writes a text file at your project's save directory with ``ProjectName_RuntimeLogger_Date.log`` like name. File structure is a ``json object array``.</br>

If you want to use that file before closing your game, you have to put ``]`` at the end. After closing it, function automatically adds it. We did like this because each entry appends that text file and we don't want to open and close file handle everytime because performance reasons.</br>

It has a delegate that can inform you about generated log. Its execution chain comes from ``AsyncTask(GameThread)`` So, you can safely create a widget from it or you can add furthere features like API callings, e-mails senders or databases drivers to write logs to other services.

- <b> Automatic Widget with Search Box and Criticality Filter</b>

## Tutorial
You can look at plugin's content folder. You will see sample blueprints.

## Platform Support
I didn't use any platform specific code. So, it should work with any platform. But I have only Windows and Android devices. So, I can't give support about other platforms especially for their editor side.

## Engine Support
I don't support ``UE4`` and older versions than ``latest version``. I will update version ``after one month`` of new major release.
