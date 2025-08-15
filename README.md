# RuntimeLogger

## Description
This plugin allows you to store, write, and visualize your runtime-generated logs from various threads and create notifications about them. It is Blueprint-exposed and designed especially for shipping builds and runtime usage.

## Workflow
- **Log Manager (Blueprint Exposed)**  
  This is a `UGameInstanceSubsystem` (Runtime Logger Subsystem).  
  It is unaffected by level changes and has a singleton/centralized design, so you can access it from anywhere.

- **Log Message (Blueprint Exposed)**  
  This function uses `TQueue` to gather your messages from every thread without race conditions.  
  Each message is a JSON object. You can define your own structure, but it automatically includes:
  - `UUID` (FGuid-based)
  - `LogTime` (`FDateTime::Now()`)
  - `LogLevel` (ERuntimeLogLevels custom enum)

  We suggest adding:
  - Plugin (or module) name
  - Class name
  - Function name  

  If you use C++, you can take advantage of `__FUNCSIG__` to automatically add class and function names (compiler feature).

- **Record Message**  
  This is a public C++ function running on `FRunnableThread`, so it won't affect game thread performance even with many logs. If there are no logs to record, the thread stays in a wait state, avoiding wasted resources.

  Logs are stored in a private `TMap<FString, FString>` accessible via getters:  
  - **Key:** UUID  
  - **Value:** Other parameters  

  It also writes a text file in your project's `Saved` directory named like:  
  `ProjectName_RuntimeLogger_Date.log`  

  The file contains a JSON object array.  
  - If you want to use it before the game closes, you must manually append a `]` at the end.
  - Upon closing, the function automatically appends it.  

  We append entries without reopening the file handle each time for performance reasons.  
  A delegate notifies you when a log is generated, and its execution chain runs via `AsyncTask(GameThread)`, allowing safe widget creation.  
  You can extend this with features like API calls, email notifications, or database writes.

- **Automatic Widget with Search Box and Criticality Filter**

## Tutorial
Check the plugin's `Content` folder for sample Blueprints.

## Platform Support
No platform-specific code is used, so it should work on any platform.  
Tested only on **Windows** and **Android** — no official support for others, especially editor-side.

## Engine Support
No UE4 or older versions are supported.  
Only the **latest** Unreal Engine version is supported.  
Updates will be provided about one month after a new major release.
