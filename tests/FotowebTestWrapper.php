<?php

require_once '../../../../../../vendor/autoload.php';
require_once 'includes/FotowebAPI/FotowebBase.inc';

class FotowebTestWrapper extends PHPUnit\Framework\TestCase {

  protected $fotowebBase;

  public function setUp() {
    parent::setUp();
    $this->fotowebBase = $this->getFotowebBase();
  }

  public function getFotowebBase() {
    $server = getenv('FOTOWEB_SERVER');
    $fullApiKey = getenv('FOTOWEB_FULLAPI_KEY');
    $guzzleConfigurator = new FotowebGuzzleTestConfigurator();
    return new FotowebBase($server, $fullApiKey, $guzzleConfigurator);
  }

}
