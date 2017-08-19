<?php

class FotowebArchiveAgentOriginalImage {

  protected $fotoweb;
  protected $asset;

  public function __construct(FotowebArchiveAgentBase $fotoweb, FotowebAsset $asset) {
    $this->fotoweb = $fotoweb;
    $this->asset = $asset;
  }

  public function getAsset() {
    return $this->asset;
  }

  public function setAsset(FotowebAsset $asset) {
    $this->asset = $asset;
  }

  public function getAssetDataFromResource($resourceUrl) {
    $data = $this->asset->getAsset($resourceUrl);
    return $data;
  }

  public function getFotoweb() {
    return $this->fotoweb;
  }

  public function getOriginalImageDownloadUrlFromResource($resourceUrl) {
    $assetData = $this->getAssetDataFromResource($resourceUrl);
    $fileId = $this->getFileIdFromAsset($assetData);
    $archivePath = $this->getArchivePath($assetData);
    $downloadUrl = $this->buildOriginalImageDownloadUrl($fileId, $archivePath);
    return $downloadUrl;
  }

  /**
   * We assume, that we only get one file result for the ArchiveAgent search
   * request.
   *
   * @param $assetData
   *
   * @return string
   */
  public function getFileIdFromAsset($assetData) {
    $filename = $assetData['filename'];
    $archivePath = $this->getArchivePath($assetData);
    $url = $this->fotoweb->getServer() . '/' . $archivePath . 'Search?Search=' . $filename;
    $response = $this->fotoweb->initiateRequest($url);
    $data = (string) $response->getBody();
    $fileId = $this->getFileIdFromXmlResult($data);
    return $fileId;
  }


  public function getArchivePath($assetData) {
    $archiveHref = $assetData['archiveHREF'];
    $archiveParts = explode('/archives/', $archiveHref, 2);
    $archivePath = array_pop($archiveParts);
    return $archivePath;
  }


  public function getFileIdFromXmlResult($data) {
    $fileId = NULL;
    libxml_use_internal_errors(TRUE);
    try {
      $xml = new SimpleXMLElement($data);
      if (!empty($xml->File[0])) {
        // Set the FileId, when it's not empty.
        if (!empty($xml->File[0]['Id'])) {
          $fileId = (string) $xml->File[0]['Id'];
        }
      }
    } catch (Exception $e) {
      throw new InvalidArgumentException($e->getMessage());
    }
    return $fileId;
  }

  public function buildOriginalImageDownloadUrl($fileId, $archivePath) {
    $url = $this->fotoweb->getServer() . '/' . $archivePath . 'Download?Id=' . $fileId;
    return $url;
  }

}
