# AnimationResourceTool
This tool generates compatibility patches for multiple animation mods in The Legend of Zelda - Tears of the Kingdom

90% of the code for this project was written by GPT 3.5 and GPT 4.0. Most of the UI is written in C++, but the basic functions to import a new mod and generate patches are written in standalone PowerShell scripts. The buttons call upon those scripts while passing arguments to them.

You can find the ChatGPT conversations that lead to the creation of this tool here:

[https://chat.openai.com/share/8cb9b8b7-eed1-4a16-aee5-fde95874057d](https://chat.openai.com/share/8cb9b8b7-eed1-4a16-aee5-fde95874057d)

[https://chat.openai.com/share/0e48a3de-49f1-430a-8ab7-0bc284bd6f7b](https://chat.openai.com/share/0e48a3de-49f1-430a-8ab7-0bc284bd6f7b)

[https://chat.openai.com/share/663eb4b6-f4a1-4055-a155-c5ba20533186](https://chat.openai.com/share/ff2f278f-0b92-44e9-9fe0-ee96e4efaf1c)

![Screenshot of the application](https://cdn.discordapp.com/attachments/788411093432401973/1163773330231406652/AnimationResourceTool_6Gj4upsBr3.png)

## Instructions:
This application is dependent on ArchLeader's tool [byml-to-yaml](https://github.com/ArchLeaders/byml_to_yaml/releases/latest), the executable "byml-to-yaml.exe" needs to be placed in the /scripts folder

A list of animation mod information is already preloaded in the tool. These are all the available mods for which patches can currently be generated.
If more animation mods are made, they can be added by importing an AnimationParam.bgyml file.

Simply select the output folder where you want the patch to be generated, choose your mods by using their "Enable" checkbox, then click "Generate patch".

The priority order works from top to bottom. The higher the ID number (at the bottom of the list), the higher the priority.

## Credits 
Icon by TheRicoLicious

Code by Lord Bubbles and ChatGPT
