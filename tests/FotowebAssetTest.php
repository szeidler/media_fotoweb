<?php

require_once 'FotowebTestWrapper.php';

/**
 * Class FotowebAssetTest
 *
 * Test to fetch specific assets and from archive traversal.
 */
class FotowebAssetTest extends FotowebTestWrapper {

  protected $fotowebAsset;

  /**
   * @inheritdoc
   */
  public function setUp() {
    parent::setUp();
    $this->fotowebAsset = new FotowebAsset($this->fotowebBase);
  }

  /**
   * Tests to fetch an asset from a fixed asset resource.
   */
  public function testFetchSpecificAsset() {
    $response = $this->fotowebAsset->initiateRequest(getenv('FOTOWEB_TEST_ASSET_HREF'));
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');

    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertArrayHasKey('previews', $data, 'Response misses previews property.');
    $this->assertGreaterThan(0, count($data['previews']), 'Response misses previews items.');
  }

  /**
   * Tests to fetch an asset from the first available archive.
   */
  public function testFetchAssetFromArchiveTraversal() {
    $fotowebArchive = new FotowebArchives($this->fotowebBase);
    $response = $fotowebArchive->initiateRequest('fotoweb/me/archives/');
    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertGreaterThan(0, count($data['data']), 'Response misses data items.');

    $assetListResource = $data['data'][0]['data'];

    $fotowebArchive = new FotowebAssetList($this->fotowebBase);
    $response = $fotowebArchive->initiateRequest($assetListResource);
    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertGreaterThan(0, count($data['data']), 'Response misses data items.');
    $this->assertArrayHasKey('href', $data['data'][0], 'Response misses item href property.');

    $assetResource = $data['data'][0]['href'];

    $response = $this->fotowebAsset->initiateRequest($assetResource);
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');
  }

  /**
   * Tests to fetch an asset from a resource.
   */
  public function testGetAsset() {
    $data = $this->fotowebAsset->getAsset(getenv('FOTOWEB_TEST_ASSET_HREF'));
    $this->assertNotEmpty($data['filename'], 'Asset has no filename information.');
  }

  /**
   * Tests to fetch an image from fixed asset.
   */
  public function testGetImageResourceFromAsset() {
    $response = $this->fotowebAsset->getAssetImageFromResource(getenv('FOTOWEB_TEST_ASSET_HREF'));
    $this->assertContains('image/', $response->getHeader('Content-Type')[0], 'Returned content type is not image.');
  }

  /**
   * Test if the isImageABetterFit Method actually works as expected.
   */
  public function testIsImageABetterFitIntegrity() {
    $originalImageWidth = 1600;
    $threshold = 1400;
    $fotowebAsset = new FotowebAsset($this->fotowebBase, $threshold);

    $currentImage = array('width' => 1600, 'square' => FALSE);
    $bestFitImage = array('width' => 1400, 'square' => FALSE);
    $this->assertTrue($fotowebAsset->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));

    $currentImage = array('width' => 1601, 'square' => FALSE);
    $bestFitImage = array('width' => 1400, 'square' => FALSE);
    $this->assertFalse($fotowebAsset->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));

    $currentImage = array('width' => 1700, 'square' => FALSE);
    $bestFitImage = array('width' => 1500, 'square' => FALSE);
    $this->assertFalse($fotowebAsset->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));

    $currentImage = array('width' => 1500, 'square' => FALSE);
    $bestFitImage = array('width' => 1650, 'square' => FALSE);
    $this->assertTrue($fotowebAsset->isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth));
  }

  /**
   * Test, that the bestFitImage returns the appropriate image.
   *
   * It is ased on themaximum file size.
   * Uses the $image['square'] = TRUE exclusion.
   */
  public function testBestFitImageFunctional() {
    $originalImageWidth = 1600;
    $threshold = 1400;
    $fotowebAsset = new FotowebAsset($this->fotowebBase, $threshold);

    $mock_previews = array(
      0 => array(
        'width' => -500,
        'square' => FALSE,
      ),
      1 => array(
        'width' => 500,
        'square' => FALSE,
      ),
      2 => array(
        'width' => 1600,
        'square' => TRUE,
      ),
      3 => array(
        'width' => 1400,
        'square' => TRUE,
      ),
      4 => array(
        'width' => 1450,
        'square' => FALSE,
      ),
      5 => array(
        'width' => 1800,
        'square' => FALSE,
      ),
      6 => array(
        'width' => 1750,
        'square' => FALSE,
      ),
    );

    $mock_previews = $fotowebAsset->orderPreviewsDescendant($mock_previews);

    $bestFitImage = reset($mock_previews);
    foreach ($mock_previews as $current_image) {
      if ($fotowebAsset->isImageABetterFit($current_image, $bestFitImage, $originalImageWidth)) {
        $bestFitImage = $current_image;
      }
    }
    $this->assertEquals(1450, $bestFitImage['width'], 'Method returned the wrong best fit image.');
  }

  /**
   * Test that the writeback function of asset metadata is working.
   */
  public function testWriteBackMetadata() {
    $resourceUrl = getenv('FOTOWEB_TEST_ASSET_HREF');
    // Append a random string, to be sure, that metadata is really updated.
    $randomMetadataString = 'Updated metadata: ' . hash('sha256', time());
    $metadata = array(40 => array('value' => $randomMetadataString));

    // Update metadata.
    $response = $this->fotowebAsset->initiateUpdateMetadataRequest($resourceUrl, $metadata);
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');

    // Check if the metadata was succesfully updated.
    $response = $this->fotowebAsset->initiateRequest($resourceUrl);
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');

    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertEquals($randomMetadataString, $data['metadata'][40]['value'], 'Updated metadata does not match the expected result');
  }

}
