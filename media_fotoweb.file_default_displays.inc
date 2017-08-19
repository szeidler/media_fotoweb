<?php

/**
 * @file
 * Provides default configuration for file displays.
 */

/**
 * Implements hook_file_default_displays().
 *
 * @return array
 *   File display configuration.
 */
function media_fotoweb_file_default_displays() {
  $file_displays = array();

  $default_image_styles = array(
    'teaser' => 'medium',
    'preview' => 'media_thumbnail',
    'default' => ''
  );

  foreach ($default_image_styles as $view_mode => $image_style) {
    $display_name = 'image__' . $view_mode . '__media_fotoweb_image';
    $file_display = new stdClass();
    $file_display->api_version = 1;
    $file_display->name = $display_name;
    $file_display->weight = 0;
    $file_display->status = TRUE;
    $file_display->settings = array(
      'image_style' => $image_style,
    );
    $file_displays[$display_name] = $file_display;
  }

  return $file_displays;
}
