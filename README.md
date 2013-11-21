# Simplicity for Pebble SDK 2.0

The Simplicity watchface from Pebble has been extended for custom features such as screen background, date format, and vibration alerts.


## Simplicity Example

The Simplicity watchface has been converted for customizable settings, and the source code can be found here:

<https://github.com/SetPebble/Simplicity2>


## SetPebble.com Instructions

First register for an account at SetPebble.com and check the "Developer" checkbox for your account.  One your "Account" page, you will see a butt for "Add Pebble App".

Here is a screenshot for creating the Simplicity sample app in SetPebble:
![Create App](https://github.com/SetPebble/Simplicity2/raw/master/design/create-app.png "Create App")
Be sure to choose "2.0" for the Pebble OS.

After adding the app, now go into the "Edit Version" page.  You should choose "JSON" for the Return Format, and remember the four-character key.  This key will be used to get the custom settings from SetPebble.com to your Pebble watchface or app.

![Edit Version](https://github.com/SetPebble/Simplicity2/raw/master/design/edit-version.png "Create App")

After editing the version, you should now creat the Version Settings.  These are the individual items that can be customized.

There are several types of settings availabe, and click on the "Detailed Instructions" button to see more details.

![Version Settings](https://github.com/SetPebble/Simplicity2/raw/master/design/edit-settings.png "Version Settings")

That's it for the SetPebble.com setup.


## Settings on Smart Phone

There are two links that are important to your Pebble app.  From your Pebble app, this link is used to edit the configurable settings:

<http://x.SetPebble.com/C45W/00000000000000000000000000000000>

The zeroes are replaced by the unique Pebble token (see the PebbleJS documentation for an explanation).

When invoked from your Pebble app, the linked smart phone sees a screen like the following:

![Settings](https://github.com/SetPebble/Simplicity2/raw/master/design/settings.png "Settings")


## Retrieve Settings

This second link is used to retrieve the customizable settings in JSON format:

<http://x.SetPebble.com/api/C45W/00000000000000000000000000000000>

Here is an example of a response:

{"1":0,"2":1,"2":0}


## Developer Dashboard

For developers who use SetPebble.com, usage records are kept and displayed in a dashboard per app and version.


## Summary

Storing your app settings will reduce the development effort for creating Pebble apps, plus give access to summarized usage records.

[SetPebble.com](http://SetPebble.com "SetPebble.com") is provided as a free service to the Pebble developer community.