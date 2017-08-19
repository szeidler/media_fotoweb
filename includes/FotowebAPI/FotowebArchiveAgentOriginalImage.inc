<?php

/**
 * FotowebArchiveAgentOriginalImage handles the original image retrieval.
 */
class FotowebArchiveAgentOriginalImage {

  protected $fotoweb;
  protected $asset;

  /**
   * FotowebArchiveAgentOriginalImage constructor.
   *
   * @param FotowebArchiveAgentBase $fotoweb
   *   Fotoweb ArchiveAgent Base object.
   * @param FotowebAsset $asset
   *   Fotoweb asset object.
   */
  public function __construct(FotowebArchiveAgentBase $fotoweb, FotowebAsset $asset) {
    $this->fotoweb = $fotoweb;
    $this->asset = $asset;
  }

  /**
   * Returns the asset.
   *
   * @return FotowebAsset
   *   Fotoweb asset.
   */
  public function getAsset() {
    return $this->asset;
  }

  /**
   * Sets the asset.
   *
   * @param FotowebAsset $asset
   *   Fotoweb asset.
   */
  public function setAsset(FotowebAsset $asset) {
    $this->asset = $asset;
  }

  /**
   * Returns the asset properties from a given resource.
   *
   * @param string $resourceUrl
   *   Url of the asset.
   *
   * @return array
   *   Asset array.
   */
  public function getAssetDataFromResource($resourceUrl) {
    $data = $this->asset->getAsset($resourceUrl);
    return $data;
  }

  /**
   * Returns the Fotoweb Base.
   *
   * @return FotowebArchiveAgentBase
   *   Fotoweb Base.
   */
  public function getFotoweb() {
    return $this->fotoweb;
  }

  /**
   * Returns the download URL for an original image.
   *
   * @param string $resourceUrl
   *   Url of the asset.
   *
   * @return string
   *   Download url for the original Image.
   */
  public function getOriginalImageDownloadUrlFromResource($resourceUrl) {
    $assetData = $this->getAssetDataFromResource($resourceUrl);
    $fileId = $this->getFileIdFromAsset($assetData);
    $archivePath = $this->getArchivePath($assetData);
    $downloadUrl = $this->buildOriginalImageDownloadUrl($fileId, $archivePath);
    return $downloadUrl;
  }

  /**
   * Returns the Archive Agent file ID from a given asset.
   *
   * We assume, that we only get one file result for the ArchiveAgent search
   * request.
   *
   * @param array $assetData
   *   Properties of an asset.
   *
   * @return string
   *   Archive Agent File Id.
   */
  public function getFileIdFromAsset(array $assetData) {
    $filename = $assetData['filename'];
    $archivePath = $this->getArchivePath($assetData);
    $url = $this->fotoweb->getServer() . '/' . $archivePath . 'Search?Search=' . $filename;
    $response = $this->fotoweb->initiateRequest($url);
    $data = (string) $response->getBody();
    $fileId = $this->getFileIdFromXmlResult($data);
    return $fileId;
  }

  /**
   * Returns the archive path from a given asset.
   *
   * @param array $assetData
   *   Properties of an asset.
   *
   * @return string
   *   Archive path.
   */
  public function getArchivePath(array $assetData) {
    $archiveHref = $assetData['archiveHREF'];
    $archiveParts = explode('/archives/', $archiveHref, 2);
    $archivePath = array_pop($archiveParts);
    return $archivePath;
  }

  /**
   * Returns the file ID from an Archive Agent XML Result.
   *
   * @param string $data
   *   XML Data.
   *
   * @return null|string
   *   File Id.
   */
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
    }
    catch (Exception $e) {
      throw new InvalidArgumentException($e->getMessage());
    }
    return $fileId;
  }

  /**
   * Constructs the original image download URL.
   *
   * @param string $fileId
   *   Given file Id.
   * @param string $archivePath
   *   Fotoweb ArchiveAgent archive path.
   *
   * @return string
   *   Original image url.
   */
  public function buildOriginalImageDownloadUrl($fileId, $archivePath) {
    $url = $this->fotoweb->getServer() . '/' . $archivePath . 'Download?Id=' . $fileId;
    return $url;
  }

}
