<?php

/**
 * @file
 * Implements hook_file_* hooks in a separated file.
 */

/**
 * Implements hook_file_operations().
 */
function media_fotoweb_file_operations() {
  $operations = array(
    'media_fotoweb_refresh' => array(
      'label' => t('Refresh Fotoweb information from source'),
      'callback' => '_media_fotoweb_cache_clear',
    ),
  );

  return $operations;
}

/**
 * Implements hook_file_delete().
 */
function media_fotoweb_file_delete($file) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme === 'fotoweb' && $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri)) {
    _media_fotoweb_flush_image($wrapper);
  }
}

/**
 * Flushes the corresponding image for the fotoweb asset.
 *
 * @param DrupalStreamWrapperInterface $wrapper
 *   FotowebStreamWrapper to flush the image from.
 */
function _media_fotoweb_flush_image($wrapper) {
  $uri = $wrapper->getLocalImagePath();

  // Delete stored original file.
  file_unmanaged_delete($uri);

  // Delete all genrated image derivatives.
  _media_fotoweb_flush_image_derivative($uri);
}

/**
 * Flushes the generated image derivatives for the fotoweb asset.
 *
 * @param $uri
 *   Drupal file URI.
 */
function _media_fotoweb_flush_image_derivative($uri) {
  module_load_include('inc', 'media_fotoweb', 'includes/media_fotoweb.helper');
  $styles = image_styles();
  foreach ($styles as $style) {
    $image_style_path = _media_fotoweb_image_style_path($style['name'], $uri);
    if (file_exists($image_style_path)) {
      file_unmanaged_delete($image_style_path);
    }
  }
}

/**
 * Clear the cached Fotoweb content for the selected files.
 *
 * @param array $fids
 *   Array of tile ids to clear the cache for.
 */
function _media_fotoweb_cache_clear($fids) {
  $fids = array_keys($fids);

  $query = new EntityFieldQuery();
  $results = $query
    ->entityCondition('entity_type', 'file')
    ->propertyCondition('uri', 'fotoweb:', 'STARTS_WITH')
    ->propertyCondition('fid', $fids)
    ->execute();

  $files = file_load_multiple(array_keys($results['file']));

  foreach ($files as $file) {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $local_path = $wrapper->getLocalImagePath();
    file_unmanaged_delete($local_path);
  }
}
