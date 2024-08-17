# Sekiro Death Counter
This OBS Plugin was build using the newest OBS Plugin Template version and targeting OBS version 30.1.2 as seen in the buildspec.json file and was also tested against said version.

## How it Works
After Installing the OBS Plugin it checks every 5 Seconds if the Sekiro process (sekiro.exe) is found. When the process is found it retrieves a Handle the Process, Gets the Deathcounter Address and reads it every 5 Seconds into the Selected Text Source.

## How to Use
1. Download the the zip archive from the [Release Tab](https://github.com/obsproject/obs-plugintemplate/releases) and extract the contents to  ```C:\Program Files\obs-studio```
2. Under Tools -> Sekiro Deathcounter Select the Text-Source which should display your Deathcount and Press Save
3.  Start Sekiro and start playing

## Todos
- Add Translation Files
- Add Support for Linux and MacOS