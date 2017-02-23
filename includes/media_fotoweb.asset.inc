<?php

/**
 * Create files from Fotoweb assets.
 */
function _media_fotoweb_add_asset_from_fotoweb() {
  if (!empty($_POST['asset'])) {
    $asset = $_POST['asset'];
    _media_fotoweb_save_file($asset);
  }
}

function _media_fotoweb_save_file($asset) {
  if (!empty($asset['href'])) {
    // We assume the resource to be valid, because it's coming from a
    // Fotoweb response.
    $is_valid_resource = TRUE;
    $handler = new FotowebHandler($asset['href'], $is_valid_resource);
    $file = $handler->getFileObject();

    if (empty($file->fid)) {
      //file_save($file);
    }
    else {
      // @todo: Update metadata for existing file.
    }

    return $file;
  }
}
