#!/bin/bash
# Simple script to install drupal for travis-ci running.

set -e $DRUPAL_TI_DEBUG

# Ensure the right Drupal version is installed.
drupal_ti_ensure_drupal

echo "Test"

# Enable simpletest module.
cd "$DRUPAL_TI_DRUPAL_DIR"
drush --yes en simpletest

# Enable composer module
echo "DRUPAL TI - Download Composer module and enable"
drush dl composer-8.x-1.x
echo "DRUPAL TI - Clear Drush cache"
drush cc drush
drush cc all
echo "DRUPAL TI - Delete cache dir"
rm -f "$DRUPAL_TI_CACHE_DIR"/HOME/.drush/cache

# Download required modules.
drush dl composer_manager
drush en -y composer_manager

drush composer-json-rebuild

# Ensure the module is linked into the code base and enabled.
drupal_ti_ensure_module

# Clear caches and run a web server.
drupal_ti_clear_caches
drupal_ti_run_server