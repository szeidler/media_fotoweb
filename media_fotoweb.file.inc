<?php

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
 * Clear the cached Fotoweb content for the selected files.
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
