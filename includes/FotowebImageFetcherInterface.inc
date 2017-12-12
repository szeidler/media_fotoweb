<?php

/**
 * Interface for Fotoweb Image Fetchers
 */
interface FotowebImageFetcherInterface {

  /**
   * Returns the image url from the Fotoweb asset resource.
   *
   * @return string|null
   *   URL the image can be fetched from.
   */
  public function getImageUrl();

  /**
   * Fetches the Fotoweb image and store it on the disk.
   *
   * @param string $url
   *   Remote URL of the Fotoweb image.
   *
   * @return bool
   *   Status of the process was successful.
   */
  public function store($url);

}
