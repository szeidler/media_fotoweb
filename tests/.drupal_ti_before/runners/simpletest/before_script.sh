#!/bin/bash
# Simple script to install drupal for travis-ci running.

set -e $DRUPAL_TI_DEBUG

# Ensure the right Drupal version is installed.
drupal_ti_ensure_drupal

# Download required modules.
drush dl composer_manager
drush en -y composer_manager

drush composer-json-rebuild
