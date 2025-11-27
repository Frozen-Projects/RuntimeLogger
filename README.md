# RuntimeLogger

## Description
This plugin captures ``UE_LOG`` based entries at runtime to store and visualize them.

## Workflow / Tutorial
**Log Manager (Blueprint Exposed)**</br>
This is a `UGameInstanceSubsystem` (Runtime Logger Subsystem). It is unaffected by level changes because its singleton design. Also you can access it from anywhere.

**Log Message (Blueprint Exposed)**</br>
After enabling this plugin, your ``FOutputDevice`` for ``UE_LOG`` will automatically change with our custom implementation. When you log something with ``LogTemp``,</br>
for example ``UE_LOG(LogTemp, Warning, TEXT("YOUR_AWASOME_LOG"))``</br>
it will be captured by this plugin, additional to ``Output Window`` showing. So, you don't have to do something special. Other categories behave as usual.
</br>
</br>
We also added a blueprint exposed function name ``Log Message``. Its message accepts ``FJsonObjectWrapper``. Because when your project groves, single sentenced logs won't be enough and you have add other sections like ``Plugin or Module Name``, ``Function Name``, ``Details`` and etc. In that case, ``JSON`` gives us more tidy logs.
</br>
If you are C++ developer, we suggest you to use ``__FUNCTION__`` parameter. (It won't work on blueprints.) Sample Use Case: </br>
````
FJsonObjectWrapper Log_Json;
Log_Json.JsonObject->SetStringField("FunctionName", FString(ANSI_TO_TCHAR(__FUNCTION__)));
````
</br>
</br>
When your log captured, our system will automatically add an ``UE5 FGUID based UUID``, ``FDateTime::Now() based LogTime`` and its ``Verbosity`` level to that JSON.

- **Visualizing Logs**
You have three options.
- Widget Method: If you will look at plugin's content folder, you can see some sample Widgets. They are blueprint exposed C++ widgets. So, you can add other components and change their styles based on your needs as long as you protect their current components and their names. Just delete ``Spawn Actor Class`` from sample blueprint actor **(it use our in house third part window system.)** and add ``UI_RL_Viewport`` to viewport. When you log something, you can see it. That's all.

- File Method: You can see a new file created at ``Project/Saved/`` like this ``ProjectName_RuntimeLogger_DATE-TIME.log``. It is json based but While your project running, it doesn't have JSON ending ``]}`` to allow adding new entries. So you must mannualy append them at the end, if you want to use it before the game closes.   - Upon closing, the function automatically appends them.

- Delegate Method: There is a delegate for captured logs. You can use them for additional purposes. For example your own widget system, mail sender, API connections and etc.

## Platform Support
No platform-specific code is used, so it should work on any platform.  
Tested only on **Windows** and **Android** — no official support for others, especially editor-side.

## Engine Support
No UE4 or older versions are supported.  
Only the **latest** Unreal Engine version is supported.  
Updates will be provided about one month after a new major release.