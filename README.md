# Media Fotoweb module for Drupal 8.x.
----------------------------------------------------------------

CONTENTS OF THIS FILE
---------------------

* Introduction
* Requirements
* Installation
* Configuration
* Maintainers


INTRODUCTION
------------

Media: Fotoweb provides an integration with Fotoware's
[Fotoweb](http://www.fotoware.com/products/fotoweb) Digital Asset Management
System. The module provides Fotoweb's Selection Widget as a Entity browser
plugin. Once an user selects an asset, the asset will be imported as a media
item and will be available to reuse in Drupal like every other media.


REQUIREMENTS
------------

This module requires the following modules:

* Media ([Drupal core](http://drupal.org/project/drupal))
* [Entity Browser](http://drupal.org/project/entity_browser)


INSTALLATION
------------

Install the module as usual, more info can be found on:
https://www.drupal.org/docs/8/extending-drupal-8/installing-drupal-8-modules


CONFIGURATION
-------------

* Go to the Fotoweb module configuration (`/admin/config/media/media-fotoweb`)
* Fill in your `Fotoweb server` and `Full API Key`


## Fotoweb Single Sign-on

The module provides Single Sign-on for your Drupal users into the Fotoweb
system. Your users will not need to login to Fotoweb anymore to select images.

* Go to the Fotoweb module configuration (`/admin/config/media/media-fotoweb`)
* Set your Fotoweb encryption secret
* Select a field on the user entity that will be used to store the Fotoweb's
username for the particular user. You can use any plain text (string) field
from your user entity.

## File storage types

Original images from Fotoweb might be unnecessary big for your website usage.
You can either store the original image or a smaller appropriate preview with
your desired maximum width (Local file size threshold). The module will
download the scaled down image with the next bigger dimensions according to
your threshold.


MAINTAINERS
-----------

Current maintainers:
- Stephan Zeidler (szeidler) - https://www.drupal.org/user/767652
