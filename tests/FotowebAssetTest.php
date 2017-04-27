<?php

require_once 'FotowebTestWrapper.php';

class FotowebAssetTest extends FotowebTestWrapper {

  protected $fotowebAsset;

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
    $this->assertEquals('image/jpeg', $response->getHeader('Content-Type')[0], 'Returned content type is not image/jpeg.');
  }

  /**
   * Test if the bestFitImage Method actually works as expected.
   */
  public function testBestFitImageIntegrity() {
    $maximum_file_size = 1000;
    $fotowebAsset = new FotowebAsset($this->fotowebBase, $maximum_file_size);

    $currentImage = array('size' => 599);
    $bestFitImage = array('size' => 600);
    $this->assertFalse($fotowebAsset->isBestFitImage($currentImage, $bestFitImage));

    $currentImage = array('size' => 600);
    $bestFitImage = array('size' => 600);
    $this->assertFalse($fotowebAsset->isBestFitImage($currentImage, $bestFitImage));

    $currentImage = array('size' => 601);
    $bestFitImage = array('size' => 600);
    $this->assertTrue($fotowebAsset->isBestFitImage($currentImage, $bestFitImage));
  }

  /**
   * Test, that the bestFitImage returns the appropriate image, based on the
   * maximum file size. Uses the $image['square'] = TRUE exclusion.
   */
  public function testBestFitImageFunctional() {
    $maximum_file_size = 1500;
    $fotowebAsset = new FotowebAsset($this->fotowebBase, $maximum_file_size);

    $mock_previews = array(
      0 => array(
        'size' => -500,
        'square' => FALSE,
      ),
      1 => array(
        'size' => 500,
        'square' => FALSE,
      ),
      2 => array(
        'size' => 1600,
        'square' => TRUE,
      ),
      3 => array(
        'size' => 1400,
        'square' => TRUE,
      ),
      4 => array(
        'size' => 1450,
        'square' => FALSE,
      ),
      5 => array(
        'size' => 1800,
        'square' => FALSE,
      ),
      6 => array(
        'size' => 1750,
        'square' => FALSE,
      ),
    );

    $bestFitImage = array('size' => 0);
    foreach ($mock_previews as $current_image) {
      if (!$current_image['square'] && $fotowebAsset->isBestFitImage($current_image, $bestFitImage)) {
        $bestFitImage = $current_image;
      }
    }
    $this->assertEquals(1450, $bestFitImage['size'], 'Method returned the wrong best fit image.');
  }

}
