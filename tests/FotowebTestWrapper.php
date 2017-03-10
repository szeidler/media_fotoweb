<?php

require_once '../../../../../../vendor/autoload.php';
require_once 'includes/FotowebAPI/FotowebBase.inc';

abstract class FotowebTestWrapper extends PHPUnit\Framework\TestCase {

  protected $fotowebBase;

  public function setUp() {
    parent::setUp();

    $server = getenv('FOTOWEB_SERVER');
    $fullApiKey = getenv('FOTOWEB_FULLAPI_KEY');
    $guzzleConfigurator = new FotowebGuzzleTestConfigurator();
    $this->fotowebBase = new FotowebBase($server, $fullApiKey, $guzzleConfigurator);
  }

}
