# Satisfactory UEVR Enhancements

This is a work-in-progress mod for Satisfactory intended to accompany Praydog's [Unreal Engine Virtual Reality Tool](https://uevr.io/), to provide VR-specific UI and interaction enhancements, to make playing Satisfactory in VR more practical and enjoyable.

*This mod does not enable VR! It adds VR controls and interactions. You will need the [Unreal Engine Virtual Reality Tool](https://uevr.io/) as well, which runs separately to Satisfactory.*

This mod is in alpha state, and is functional for core Satisfactory gameplay elements. However, it is still experimental, so things may change, and there is still much to improve and optimise.

See the [Controls](Controls.md) for tips on usage, and check the [To Do](To Do.md) for current progress and future goals.

## First Time Installation and Setup

- Install this mod either via [Satisfactory Mod Manager](https://ficsit.app/) or by downloading a [release zip from github](https://github.com/dortamur/satisfactory-uevr-enhancements/releases) and unzip it to your Satisfactory Mods folder
- Download the UEVR tool from its [releases on github](https://github.com/praydog/UEVR/releases)
- Unzip it to a folder somewhere, and create a shortcut to `UEVRInjector.exe`
- Download this [Satisfactory profile for UEVR](UEVR/FactoryGame-Win64-Shipping.zip)
- Run `UEVRInjector.exe` and select "Import Config". Select the file `FactoryGame-Win64-Shipping.zip` file downloaded in the previous step
- Now you're ready to go!

## Starting Satisfactory with UEVR

- Run Satisfactory, and load into your game
- Make sure your VR headset is connected and ready
- Run the UEVR tool, select `FactoryGame-Win64-Shipping` from the Inject menu, and inject into the game

## VR Headset Tips

- **Quest** or similar headset using **Virtual Desktop**: Select "OpenXR" in the UEVR Tool before injecting it
- PC **Steam VR** headset: Select "OpenVR" in the UEVR Tool before injecting it
- ...let me know what works best for your setup!

## Troubleshooting

### Windows Defender/other virus scanner things UEVR is an evil virus!!1!
The UEVR Tool is a very recent open source tool that injects code into your other apps. Injecting code is what viruses do to do bad things, but in this case UEVR needs to do this to enable VR in games like Satisfactory that don't otherwise support it.
You may need to tell your virus scanner to ignore/trust UEVR so it doesn't panic and hide it away from you.
### Satisfactory says "Ready for UEVR Injection..." but it's not in VR
Did you set up UEVR, and "Inject" it into `FactoryGame-Win64-Shipping`? If so and you're still having problems, check the [UEVR site](https://uevr.io/) or the channels in their Flat2VR discord. Also try using it with other Unreal Engine games.
### Wait - I can use UEVR for other games?
Well, yes - it's the "Unreal Engine Virtual Reality Tool". There's a lot of info about how to best configure it and how well it works for other games on the [Flat2VR Discord](https://flat2vr.com/). But the factory must grow...
### The in-game screens move about when I move my right controller!
UEVR does some magic hacks to work, and one of the side-effects is, to be able to "aim" with your controller, it tricks the game into thinking the game camera is there - which affects where the screen-space UI ends up. That is - Yes, we know about it, No, we don't have a fix right now.
### So how do I click on stuff if the screen keeps moving?
Hold your right controller still so the screen is in front of you, and then use your *left* controller to point and click at things shown on the screen. Trigger emulates a mouse left-click, and Grip emulates a right-click.
### I still can't click on some stuff!!
Yeeeeaaah, it's not perfect, is sometimes a bit flaky, and some buttons just refuse to be clicked. You can try pressing "A" on the *right* controller, which seems to be some built-in left-click like event that sometimes work when the left trigger doesn't. 🤷‍♂️
You may need to drop back to flat-mode for manual mouse fiddling occasionally (how you do this depends on your headset and how you're connecting).
### I click stuff to change settings, but when I close the screen the settings reset
This can happen when the Satisfactory game window doesn't have focus. Did you Alt-tab to Satisfactory Calculator on another screen? Make sure Satisfactory has the active focus.
### Ok, I'm in VR but I can't aim straight and I can't build stuff!
It sounds like you didn't import the provided [Satisfactory profile for UEVR](UEVR/FactoryGame-Win64-Shipping.zip) , or have reset the settings.
### Aieee! Smooth turn makes me want to hurl!
*...adds "Snap Turn Option" to the To Do list...*
Sorry. You can physically turn as an alternate turn option - swivel chairs and wireless headsets are good for this!
### I can't drive vehicles!
Nope! Not yet... At least there's a button to get out! (Right Grip)

## Credits

- Almine is working on his own VR prototype, and we have been sharing techniques and code. Check out his [Satisfactory VR Plugin](https://github.com/Almine2/SatisfactoryVRPlugins) project to see what he has been experimenting with.
- Rafi has provided [custom icons](https://github.com/rccrossde/Satisfactory_VRicons) and concept mock-ups
- Satisfactory Modding Community, and their [Modding Documentation](https://docs.ficsit.app/satisfactory-modding/) as well as their help on the [Satisfactory Modding Discord Server](https://discord.gg/xkVJ73E).
- [Coffee Stain Studios](https://www.coffeestainstudios.com/) for making such an awesome game