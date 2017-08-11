# Media Fotoweb
  
Media: Fotoweb provides an integration with Fotoware's [Fotoweb](http://www.fotoware.com/products/fotoweb) Digital Asset Management System. The module provides Fotoweb's Selection Widget as a Media browser plugin. Once an user selects an asset, the asset will be imported as a managed file entity using its own Drupal file streamwrapper.

## Features

* Browse and search through the Fotoweb assets
* Upload new assets to Fotoweb
* Selected images assets will be stored locally in your Drupal site
* Asset metadata will be stored an can be matched with existing file_entity fields
* Site can optionally use scaled down renditions of your image asset, to save disc space on your server

## Requirements

* Media 7.x-2.x and its dependencies
* Composer support for utilizing [Guzzle](http://docs.guzzlephp.org/en/latest/) via:
    * [Composer template for Drupal projects](https://github.com/drupal-composer/drupal-project)
    * [Composer Manager](https://www.drupal.org/project/composer_manager)
    * [Composer Vendor Autoload](https://www.drupal.org/project/composer_autoloader)
    * [Composer Autoload](https://www.drupal.org/project/composer_autoload)
    * or similar
* When using Manual Crop: Enable submodule and patch manualcrop with [this patch](https://gist.githubusercontent.com/szeidler/f9445e1b2bc140d48f019da8062c2c23/raw/f75d34bf56c18463d57b9d96801181916b91ef62/manualcrop-make_selection_path_alterable.patch) to make Manual Crop find the correct selection for a fotoweb asset

## Troubleshooting

* Nginx returns 404 for image style derivatives *

When you use a advanced nginx configuration, like [drupal-with-nginx](https://github.com/perusio/drupal-with-nginx), Fotoweb image style derivatives will not be created, because Nginx only look for the physical file existence. You will need to adjust [your Nginx configuration](https://gist.github.com/szeidler/5b44556691467c4be408314c9c070671) to let Drupal process fotoweb image styles.

```
location ~* /files/media-fotoweb/styles/ {
  access_log off;
  expires 30d;
  try_files $uri @drupal;
}
```
