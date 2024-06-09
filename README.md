# Satisfactory UEVR Enhancements

This is a work-in-progress mod for Satisfactory intended to accompany Praydog's [Unreal Engine Virtual Reality Tool](https://uevr.io/), to provide VR-specific UI and interaction enhancements, to make playing Satisfactory in VR more practical and enjoyable.

*This mod does not enable VR! It adds VR controls and interactions. You will need the [Unreal Engine Virtual Reality Tool](https://uevr.io/) as well, which runs separately to Satisfactory.*

This mod is in alpha state, and is functional for core Satisfactory gameplay elements. However, it is still experimental, so things may change, and there is still much to improve and optimise.

See the [Controls](Controls.md) for tips on usage, and check the [To Do](To Do.md) for current progress and future goals.

For further information, see:

- [Getting Started](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Getting-Started)
  - [First Time Installation and Setup](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Getting-Started#first-time-installation-and-setup)
  - [Starting Satisfactory with UEVR](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Getting-Started#starting-satisfactory-with-uevr)
  - [Upgrading](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Getting-Started#upgrading)
- [Controls](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Controls)
- [Troubleshooting](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Troubleshooting)

## VR Headset Tips

- **Quest** or similar headset using **Virtual Desktop**: Select "OpenXR" in the UEVR Tool before injecting it
- **Quest** or **Rift** using **Oculus Link**: Select "OpenXR" in the UEVR Tool before injecting it
- PC **Steam VR** headset: Select "OpenVR" in the UEVR Tool before injecting it
- VR is a *performance hog*. If your frame-rate is super-terrible, try:
	- Turn off "Global Illumination"!
	- Dropping your VR render resolution down
	- Drop your DLSS down to "Performance" (for NVIDIA cards)
	- Check you're using the right one out of OpenVR/OpenXR
- See the [Performance](https://github.com/dortamur/satisfactory-uevr-enhancements/wiki/Performance) page for some more performance tips, and to see reported performance that others are getting
- Let me know what works best for your setup!

## Credits

- Almine for sharing UE5 and Satisfactory mod techniques and code, and working on his own [Satisfactory VR prototype](https://github.com/Almine2/SatisfactoryVRPlugins)
- Rafi for providing [custom icons](https://github.com/rccrossde/Satisfactory_VRicons) and other UI design help
- [Praydog](https://github.com/praydog) and his awesome [UEVR Tool](https://uevr.io/)
- [Satisfactory Modding Community](https://discord.gg/xkVJ73E) for their help, and the [Modding Documentation](https://docs.ficsit.app/satisfactory-modding/)
- [Flat2VR Community](https://discord.com/channels/747967102895390741/1062167971956531340) for their enthusiastic support and feedback.
- [Coffee Stain Studios](https://www.coffeestainstudios.com/) for making such an awesome game
- [Oculus Controller Icons](https://www.figma.com/community/file/1277443078935776593) by Felix Z