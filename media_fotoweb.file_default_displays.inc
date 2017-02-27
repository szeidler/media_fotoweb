<?php

/**
 * Implements hook_file_default_displays().
 */
function media_fotoweb_file_default_displays() {
  $file_displays = array();

  $file_display = new stdClass();
  $file_display->api_version = 1;
  $file_display->name = 'image__preview__media_fotoweb_preview';
  $file_display->weight = 0;
  $file_display->status = TRUE;
  $file_display->settings = array(
    'image_style' => 'media_thumbnail',
  );
  $file_displays['image__preview__media_fotoweb_preview'] = $file_display;

  return $file_displays;
}
