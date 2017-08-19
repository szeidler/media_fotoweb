<?php

require_once 'FotowebArchiveAgentTestWrapper.php';

/**
 * Tests the search and fetch of the original image using the ArchiveAgent API.
 */
class FotowebArchiveAgentOriginalImageTest extends FotowebArchiveAgentTestWrapper {

  protected $originalImage;

  public function setUp() {
    parent::setUp();
    $fotowebTestWrapper = new FotowebTestWrapper();
    $this->originalImage = new FotowebArchiveAgentOriginalImage($this->fotowebBase, new FotowebAsset($fotowebTestWrapper->getFotowebBase()));
  }

  /**
   * Test, if archive will be extracted correctly from asset data..
   */
  public function testGetArchivePath() {
    $hrefs = array(
      array('archiveHREF' => '/fotoweb/archives/5000-Testarchive/'),
      array('archiveHREF' => '/fotoweb/archives/5000-Testarchive/nestedArchive/'),
      array('archiveHREF' => '/fotoweb/archives/5000-Testarchive/archives/childArchive/'),
    );
    foreach ($hrefs as $href) {
      $archivePath = $this->originalImage->getArchivePath($href);
      $this->assertNotContains('fotoweb/archives', $archivePath, 'Archive Path still has other path components included.');
      $this->assertStringEndsWith('/', $archivePath, 'Archive Path does not end with a path separator.');
    }
  }

  /**
   * Test that the fileId can be extracted from a ArchiveAgent search request.
   */
  public function testGetFileIdFromXmlResult() {
    $xml = '<?xml version="1.0"?><FileList Version="1.1" CreatorApplication="FotoWeb/8.0" Created="Thu, 27 Apr 2017 15:29:10 GMT" TotalHits="1" ReturnedHits="1" X-Source-Archive="5000" SearchTime="3.61265927823807" ProcessingTime="218.533378388205"><File Name="IMG_2839.JPG" Id="FBB1F7E7B9DDB53652FA37E4B9CEC289B634060C2C1880C7610C86D2B3AAFE54D77CF16D74E9F75C3817F339AA699934443249038ADCB24D66A7BFE213A0CF5CF0B6FD8F6B8C8AA30E70F1F94B16DCC68F3CF5B31C30941FEBC8CB6A45573C927AE25313716A9B1409B7C2B7FCDAAE13005FCAF2DC4385EAC65F17DD4A5B9E18D3F25FA6F0386D66A3C618381AC2C428516547C5A1D2D8340D4223A632946CD753642B745B399AF5" X-Permalink="/fotoweb/archives/5000-Test/Test_indekserte/IMG_2839.JPG.info" X-FoxToken="39835088DAE8A1165DFAFDB65B30DCABC729235A0A1C0C084DBCFE0DDBCC7167CA15992BDE2FBA91B121FC3A16080EB26CB7662B67EE79A37C787096205C7CE23ACBD84103DCC2814B76434B381826E2FF4716DD0D207FB2A97732CCAFC251E3A555D5CDEF4E6B9E2ADF543A28B031354F8B869893CACB3C109213EBA2696B61A4ECBDA2C080BBDBCE5FE4907B0253D405A66CD43674EED1E0492F3991906B4141807E8295841ECD8E1933D358FCCCF86DCC2C505C183817" X-TimeStamp="Mon, 13 Feb 2017 14:31:29 GMT"><FileInfo><Path>\\NSFFOTOWARE02\Bilder\Test_indekserte</Path><Created>Mon, 13 Feb 2017 14:33:50 GMT</Created><LastModified>Mon, 13 Feb 2017 14:31:29 GMT</LastModified><FileSize>4606976</FileSize><MimeType>image/jpeg</MimeType></FileInfo><MetaData><PixelWidth>5184</PixelWidth><PixelHeight>3456</PixelHeight><Resolution>72.00</Resolution><ColorSpace>Rgb</ColorSpace><Text><Field Id="IPTC2:20" Name="Emne">Sykehus</Field><Field Id="IPTC2:20" Name="Emne">Eksteriør</Field><Field Id="IPTC2:20" Name="Emne">Bygninger og steder</Field><Field Id="IPTC2:55" Name="Dato">2017-02-13</Field><Field Id="IPTC2:65" Name="Erstellt m. (Progr.)">FotoWare FotoStation</Field><Field Id="IPTC2:80" Name="Fotograf/Byline">Sykehuset Østfold</Field><Field Id="IPTC2:120" Name="Bildeinfo">Sykehuset Østfold på Kalnes.</Field><Field Id="IPTC2:200" Name="Bildetype">Illustrajon</Field><Field Id="IPTC2:209" Name="Serie Ja/Nei">Ja</Field><Field Id="IPTC2:320" Name="Wertung">0</Field><Field Id="IPTC2:325" Name="ICC Profil">sRGB IEC61966-2.1</Field><Field Id="IPTC2:330" Name="Make">Canon</Field><Field Id="IPTC2:331" Name="Model">Canon EOS 600D</Field><Field Id="IPTC2:350" Name="Original date">2015-09-04T12:45:01:00</Field><Field Id="IPTC2:352" Name="ISO Speed Ratings">100</Field><Field Id="IPTC2:357" Name="Serial Number">123063035640</Field></Text></MetaData></File></FileList>';
    $fileId = $this->originalImage->getFileIdFromXMLResult($xml);
    $this->assertNotEmpty($fileId, 'FileId was empty.');

    $xml = '<?xml version="1.0"?><FileList Version="1.1" CreatorApplication="FotoWeb/8.0" Created="Thu, 27 Apr 2017 15:29:10 GMT" TotalHits="1" ReturnedHits="1" X-Source-Archive="5000" SearchTime="3.61265927823807" ProcessingTime="218.533378388205"><File Name="IMG_2839.JPG" X-Permalink="/fotoweb/archives/5000-Test/Test_indekserte/IMG_2839.JPG.info" X-FoxToken="39835088DAE8A1165DFAFDB65B30DCABC729235A0A1C0C084DBCFE0DDBCC7167CA15992BDE2FBA91B121FC3A16080EB26CB7662B67EE79A37C787096205C7CE23ACBD84103DCC2814B76434B381826E2FF4716DD0D207FB2A97732CCAFC251E3A555D5CDEF4E6B9E2ADF543A28B031354F8B869893CACB3C109213EBA2696B61A4ECBDA2C080BBDBCE5FE4907B0253D405A66CD43674EED1E0492F3991906B4141807E8295841ECD8E1933D358FCCCF86DCC2C505C183817" X-TimeStamp="Mon, 13 Feb 2017 14:31:29 GMT"><FileInfo><Path>\\NSFFOTOWARE02\Bilder\Test_indekserte</Path><Created>Mon, 13 Feb 2017 14:33:50 GMT</Created><LastModified>Mon, 13 Feb 2017 14:31:29 GMT</LastModified><FileSize>4606976</FileSize><MimeType>image/jpeg</MimeType></FileInfo><MetaData><PixelWidth>5184</PixelWidth><PixelHeight>3456</PixelHeight><Resolution>72.00</Resolution><ColorSpace>Rgb</ColorSpace><Text><Field Id="IPTC2:20" Name="Emne">Sykehus</Field><Field Id="IPTC2:20" Name="Emne">Eksteriør</Field><Field Id="IPTC2:20" Name="Emne">Bygninger og steder</Field><Field Id="IPTC2:55" Name="Dato">2017-02-13</Field><Field Id="IPTC2:65" Name="Erstellt m. (Progr.)">FotoWare FotoStation</Field><Field Id="IPTC2:80" Name="Fotograf/Byline">Sykehuset Østfold</Field><Field Id="IPTC2:120" Name="Bildeinfo">Sykehuset Østfold på Kalnes.</Field><Field Id="IPTC2:200" Name="Bildetype">Illustrajon</Field><Field Id="IPTC2:209" Name="Serie Ja/Nei">Ja</Field><Field Id="IPTC2:320" Name="Wertung">0</Field><Field Id="IPTC2:325" Name="ICC Profil">sRGB IEC61966-2.1</Field><Field Id="IPTC2:330" Name="Make">Canon</Field><Field Id="IPTC2:331" Name="Model">Canon EOS 600D</Field><Field Id="IPTC2:350" Name="Original date">2015-09-04T12:45:01:00</Field><Field Id="IPTC2:352" Name="ISO Speed Ratings">100</Field><Field Id="IPTC2:357" Name="Serial Number">123063035640</Field></Text></MetaData></File></FileList>';
    $fileId = $this->originalImage->getFileIdFromXMLResult($xml);
    $this->assertNull($fileId, 'FileId was not null.');
  }

  /**
   * Test that an exception is thrown when invalid XML was used.
   *
   * @expectedException InvalidArgumentException
   * @expectedExceptionMessage String could not be parsed as XML
   */
  public function testGetFileIdWithInvalidXml() {
    $xml = '<?xml version="1.0"?>Malformed XML</FileList>';
    $fileId = $this->originalImage->getFileIdFromXMLResult($xml);
    $this->assertNull($fileId, 'FileId was not null.');
  }

  /**
   * Tests to fetch an original image download url from fixed asset.
   */
  public function testGetOriginalImageDownloadUrlFromResource() {
    $assetData = $this->originalImage->getAssetDataFromResource(getenv('FOTOWEB_TEST_ASSET_HREF'));
    $this->assertNotEmpty($assetData, 'Asset data was empty.');
    $this->assertNotEmpty($assetData['filename'], 'Filename was empty.');

    $url = $this->originalImage->getOriginalImageDownloadUrlFromResource(getenv('FOTOWEB_TEST_ASSET_HREF'));
    $this->assertNotEmpty($url, 'Download url was empty.');

    $response = $this->originalImage->getFotoweb()->initiateRequest($url);
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');
    $this->assertContains('image/', $response->getHeader('Content-Type')[0], 'Returned content type is not image.');
  }

}
