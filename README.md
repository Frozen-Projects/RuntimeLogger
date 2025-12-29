# RuntimeLogger

# Description
This plugin captures ``UE_LOG(LogTemp)`` and ``Blueprint Print String (if logging enabled)`` based logs at runtime to store and visualize them. It won't work on editor only logs.

## STRUCTURE
1. Runtime Logger Engine Subsystem: It is responsible for log management and automatically attaches ``FOutputDevice`` to catch logs that come from ``UE_LOG(LogTemp)`` and/or ``Print String`` at runtime.

### Why UEngineSubsystem ?
Because ``Deinitialize()`` and ``BeginDestroy (inherited from UObject)`` functions of ``UGameInstanceSubsystem`` run ``before actual shutdown`` and prematurely clean log file buffer. If project has a log that comes from ``EndPlay`` event, system re-open that buffer again and this causes multiple log files.</br></br>

``UEngineSubsystem`` has two benefits.
1. Level changes can't affect it.
2. While you are in editor, each ``play``session doesn't open a new file but use existing one. System cleans log file only when you close the engine or your packaged project.

## TUTORIAL
* You don't have to do something special. Just enable plugin from your editor's ``Plugins`` window and that's all. System automatically changes your default ``FOutputDevice`` with our custom one and starts to catch ``UE_LOG(LogTemp, Warning, TEXT("YOUR_AWASOME_LOG"))`` and ``Blueprint Print String (if logging enabled, because it uses an internal UE_LOG(LogBlueprintUserMessages))`` based logs additional to appearing on ``Output Window``.
* How can you visualize them depends on your imagination. You can look at plugin's ``Content`` folder for a sample.
* To access log database and log management functions, just get RuntimeLogger Engine Subystem from blueprints and call ``ResetLogs``, ``GetLogDb``, ``GetLogFilePath``, ``GetLog``, ``MemoryToJson_BP``
* There are two UPROPERTYs that named as ``bAllowSameMessage`` and ``SameMessageInterval``. If disable ``bAllowSameMessage (default disabled)``, it won't record same message (message and verbosity should be the same) that comes within specified interval.
* When your log captured, our system will automatically add an ``UE5 FGUID based UUID``, ``FDateTime::Now() based LogTime``, and its ``Verbosity level`` to that ``JSON``.

## HINTS
* Other ``Log Categories`` than ``LogTemp``and ``LogBlueprintUserMessages`` won't be captured. Because engine's itself logs every thing internally and we want to capture only developer's logs.
* We also added a blueprint exposed function that named as ``Log Message``. Its message accepts ``FJsonObjectWrapper``. Because when your project groves, single sentenced logs won't be enough and you have to add other informations like ``Plugin or Module Name``, ``Function Name``, ``Details`` and etc. In that case, ``JSON`` gives us more tidy logs.<br>
If you are C++ developer **(It won't work on blueprints.)** , we suggest you to use ``__FUNCTION__`` parameter. Because it automatically adds function and its owner class' name. It is a compiler feature from ``MSVC``. Sample Use Case:<br>

````
FJsonObjectWrapper Log_Json;
Log_Json.JsonObject->SetStringField("FunctionName", FString(ANSI_TO_TCHAR(__FUNCTION__)));

Output Log: "{"FunctionName": "YourClass:YourFunction"}
````

## Visualizing Logs
You have three options.<br>
- **Widget Method:** If you will look at plugin's content folder, you can see some sample Widgets. They are blueprint exposed C++ widgets. So, you can add other components and change their styles based on your needs as long as you protect their current components and their names. Just delete ``Spawn Actor Class`` from sample blueprint actor **(it use our in house third part window system.)** and add ``UI_RL_Viewport`` to viewport. When you log something, you can see it. That's all.

- **File Method:** You can see a new file created at ``Project/Saved/`` like this ``ProjectName_RuntimeLogger_DATE-TIME.log``. It is json based but While your project running, it doesn't have JSON ending ``]}`` to allow adding new entries. So you must mannualy append them at the end, if you want to use it before the game closes.   - Upon closing, the function automatically appends them.

- **Delegate Method:** There is a delegate for captured logs. You can use them for additional purposes. For example your own widget system, mail sender, API connections and etc.

## Platform Support
We haven't used any platform specific code. So it should work on any platform. But we tested it only on **Windows** and **Android** platforms. We don't have other OS.</br>
It works on ``editor`` and ``packaged`` **runtime** but doesn't work on ``editor`` only side.

## Engine Support
No UE4 or older versions are supported.  
Only the **latest** Unreal Engine version is supported.  
Updates will be provided about one month after a new major release.