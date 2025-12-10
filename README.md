# RuntimeLogger

# Description
This plugin captures ``UE_LOG`` and ``blueprint Print String (if logging enabled)`` based entries at runtime to store and visualize them.

# Workflow / Tutorial

## Log Manager
``Runtime Logger Subsystem (UGameInstanceSubsystem)`` is our log manager. It is a singleton object and binded to game instance. So, level changes won't affect it.

## Log Message
After enabling this plugin, default ``FOutputDevice`` for ``UE_LOG`` will automatically change with our custom implementation. When you log something with ``PrintString`` or ``UELOG(LogTemp,...)``, for example:<br>
````
UE_LOG(LogTemp, Warning, TEXT("YOUR_AWASOME_LOG"))
````
plugin will captures it, additional to appearing on ``Output Window``.<br>

To use it with ``PrintString``, you have to enable "Log" option. Because it uses an internal ``UE_LOG(LogBlueprintUserMessages)`` that can be captured by this plugin.

So, you don't have to do something special. Other ``Log Categories`` than ``LogTemp``and ``LogBlueprintUserMessages`` won't be captured. Because engine's itself logs every thing internally and we want to capture only developer's logs.<br>

We also added a blueprint exposed function that named ``Log Message``. Its message accepts ``FJsonObjectWrapper``. Because when your project groves, single sentenced logs won't be enough and you have to add other informations like ``Plugin or Module Name``, ``Function Name``, ``Details`` and etc. In that case, ``JSON`` gives us more tidy logs.<br>
If you are C++ developer **(It won't work on blueprints.)** , we suggest you to use ``__FUNCTION__`` parameter. Because it automatically adds function and its owner class' name. It is a compiler feature from ``MSVC``. Sample Use Case:<br>
````
FJsonObjectWrapper Log_Json;
Log_Json.JsonObject->SetStringField("FunctionName", FString(ANSI_TO_TCHAR(__FUNCTION__)));

Output Log: "{"FunctionName": "YourClass:YourFunction"}
````
When your log captured, our system will automatically add an ``UE5 FGUID based UUID``, ``FDateTime::Now() based LogTime``, and its ``Verbosity level`` to that ``JSON``.

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