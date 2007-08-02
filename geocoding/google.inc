<?php

// Returns lower-case of iso codes for countries supported by Google's geocoder
function google_geocode_country_list() {
  static $countries;
  
  if (!count($countries)) {
    $countries = array(
      'br',
      'ca',
      'cz',
      'de',
      'fr',
      'hu',
      'in',
      'pl',
      'pt',
      'sg',
      'tw',
      'us',
      'uk'
    );
  }
  
  return $countries;
}

// Return general info
function google_geocode_info() {
  static $info;
  
  if (!count($info)) {
    $info = array('name' => 'Google Maps', 'url' => 'http://maps.google.com', 'tos' => 'http://www.google.com/help/terms_local.html', 'general' => TRUE);
  }
  
  return $info;
}

function google_geocode_location($location = array()) {
  
  switch ($location['country']) {
    case 'uk':
      if ($location['street'] && $location['city']) {
        unset($location['postal_code']);
      }
      elseif ($location['postal_code']) {
        unset($location['city']);
        unset($location['street']);
        unset($location['additional']);
      }
      else {
        unset($location['postal_code']);
      }
      $query = location_address2singleline($location);
      break;
    default:
      $query = location_address2singleline($location);
      break;
  }
  
  if ($location['country'] == 'uk') {

  }

  $service_url = 'http://maps.google.com/maps/geo?output=xml&key='. variable_get('location_geocode_google_apikey', '') .'&q=';
  
  $http_reply = drupal_http_request($service_url . urlencode($query));
  
  $status_code_match = array();
  preg_match('/<code>(.*)<\/code>/', $http_reply->data, $status_code_match);
  $status_code = $status_code_match[1];
  if ($status_code != 200) {
    return NULL;
  }
  
  $accuracy_code_match = array();
  preg_match('/Accuracy="([0-9])"/', $http_reply->data, $accuracy_code_match);
  $accuracy_code = $accuracy_code_match[1];
  if ($accuracy_code < 3) {
    return NULL;
  }
  
  $latlon_match = array();
  preg_match('/<coordinates>(.*)<\/coordinates>/', $http_reply->data, $latlon_match);
  
  $latlon_exploded = explode(',', $latlon_match[1]);
  
  return array('lat' => $latlon_exploded[1], 'lon' => $latlon_exploded[0]);
}


function google_geocode_settings() {
  $form = array();

  $form['location_geocode_google_apikey'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Maps API Key'),
    '#size' => 64,
    '#maxlength' => 128,
    '#default_value' => variable_get('location_geocode_google_apikey', ''),
    '#description' => t('In order to use the Google Maps API geocoding web-service, you will need a Google Maps API Key.  You can obtain one at the !sign_up_link for the !google_maps_api.  PLEASE NOTE: You will <em>not</em> have to re-enter your API key for each country for which you have selected Google Maps for geocoding.  This setting is global.', array('!sign_up_link' => '<a href="http://www.google.com/apis/maps/signup.html">sign-up page</a>', '!google_maps_api' => '<a href="http://www.google.com/apis/maps/">Google Maps API</a>'))
  );

  return $form;
}

?>