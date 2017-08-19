<?php

/**
 * @file
 * Handles the asset file save after selecting it in the MediaBrowser.
 */

/**
 * Create files from Fotoweb assets.
 */
function _media_fotoweb_add_asset_from_fotoweb() {
  if (!empty($_POST['asset'])) {
    $asset = $_POST['asset'];
    $file = _media_fotoweb_file_save($asset);
    drupal_json_output($file);
  }
}

/**
 * Prepare the file object and save it.
 *
 * @param array $asset
 *   Asset array with information from the selection widget.
 *
 * @return bool|object
 *   Drupal file object or FALSE on failure.
 */
function _media_fotoweb_file_save($asset) {
  if (!empty($asset['href'])) {
    // We assume the resource to be valid, because it's coming from a
    // Fotoweb response.
    $is_valid_resource = TRUE;
    $handler = new FotowebHandler($asset['href'], $is_valid_resource);
    $file = $handler->getFileObject();

    // Inject asset properties to file object.
    $file->filename = $asset['filename'];
    $file->timestamp = strtotime($asset['modified']);

    // Inject additional file metadata to file object.
    $file->metadata['asset_created'] = strtotime($asset['created']);
    $file->metadata['asset_changed'] = strtotime($asset['modified']);
    $file->metadata['asset_resource'] = $asset['href'];
    $file->metadata['asset_metadata'] = $asset['metadata'];

    // $file->filesize would be overwritten in file_save() and fails with
    // remote streamwrappers. So save it as a metadata property.
    $file->metadata['asset_original_filesize'] = $asset['filesize'];

    // Update the files metadata when it should be updated on every selection
    // or when it's a new asset.
    if (variable_get('media_fotoweb_asset_update_trigger') === 'reused' || empty($file->fid)) {
      file_save($file);
    }

    return $file;
  }
}
