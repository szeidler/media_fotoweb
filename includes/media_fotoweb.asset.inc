<?php

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
 * @param $asset
 *
 * @return bool|object
 */
function _media_fotoweb_file_save($asset) {
  if (!empty($asset['href'])) {
    // We assume the resource to be valid, because it's coming from a
    // Fotoweb response.
    $is_valid_resource = TRUE;
    $handler = new FotowebHandler($asset['href'], $is_valid_resource);
    $file = $handler->getFileObject();

    // Inject asset properties to file object.
    $file->filesize = $asset['filesize'];
    $file->filename = $asset['filename'];
    $file->timestamp = strtotime($asset['modified']);

    // Inject additional file metadata to file object.
    $file->metadata['asset_created'] = strtotime($asset['created']);
    $file->metadata['asset_changed'] = strtotime($asset['modified']);
    $file->metadata['asset_resource'] = $asset['href'];
    $file->metadata['asset_metadata'] = $asset['metadata'];

    if (empty($file->fid)) {
      file_save($file);
    }
    else {
      // TODO Update metadata for existing file.
    }

    return $file;
  }
}
