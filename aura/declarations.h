#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <lvgl.h>

LV_FONT_DECLARE(lv_font_montserrat_latin_12);
LV_FONT_DECLARE(lv_font_montserrat_latin_14);
LV_FONT_DECLARE(lv_font_montserrat_latin_16);
LV_FONT_DECLARE(lv_font_montserrat_latin_20);
LV_FONT_DECLARE(lv_font_montserrat_latin_42);

// Font selection based on language
const lv_font_t* get_font_12() {
  return &lv_font_montserrat_latin_12;
}

const lv_font_t* get_font_14() {
  return &lv_font_montserrat_latin_14;
}

const lv_font_t* get_font_16() {
  return &lv_font_montserrat_latin_16;
}

const lv_font_t* get_font_20() {
  return &lv_font_montserrat_latin_20;
}

const lv_font_t* get_font_42() {
  return &lv_font_montserrat_latin_42;
}

// Weather icons
LV_IMG_DECLARE(icon_blizzard);
LV_IMG_DECLARE(icon_blowing_snow);
LV_IMG_DECLARE(icon_clear_night);
LV_IMG_DECLARE(icon_cloudy);
LV_IMG_DECLARE(icon_drizzle);
LV_IMG_DECLARE(icon_flurries);
LV_IMG_DECLARE(icon_haze_fog_dust_smoke);
LV_IMG_DECLARE(icon_heavy_rain);
LV_IMG_DECLARE(icon_heavy_snow);
LV_IMG_DECLARE(icon_isolated_scattered_tstorms_day);
LV_IMG_DECLARE(icon_isolated_scattered_tstorms_night);
LV_IMG_DECLARE(icon_mostly_clear_night);
LV_IMG_DECLARE(icon_mostly_cloudy_day);
LV_IMG_DECLARE(icon_mostly_cloudy_night);
LV_IMG_DECLARE(icon_mostly_sunny);
LV_IMG_DECLARE(icon_partly_cloudy);
LV_IMG_DECLARE(icon_partly_cloudy_night);
LV_IMG_DECLARE(icon_scattered_showers_day);
LV_IMG_DECLARE(icon_scattered_showers_night);
LV_IMG_DECLARE(icon_showers_rain);
LV_IMG_DECLARE(icon_sleet_hail);
LV_IMG_DECLARE(icon_snow_showers_snow);
LV_IMG_DECLARE(icon_strong_tstorms);
LV_IMG_DECLARE(icon_sunny);
LV_IMG_DECLARE(icon_tornado);
LV_IMG_DECLARE(icon_wintry_mix_rain_snow);

// Weather Images
LV_IMG_DECLARE(image_blizzard);
LV_IMG_DECLARE(image_blowing_snow);
LV_IMG_DECLARE(image_clear_night);
LV_IMG_DECLARE(image_cloudy);
LV_IMG_DECLARE(image_drizzle);
LV_IMG_DECLARE(image_flurries);
LV_IMG_DECLARE(image_haze_fog_dust_smoke);
LV_IMG_DECLARE(image_heavy_rain);
LV_IMG_DECLARE(image_heavy_snow);
LV_IMG_DECLARE(image_isolated_scattered_tstorms_day);
LV_IMG_DECLARE(image_isolated_scattered_tstorms_night);
LV_IMG_DECLARE(image_mostly_clear_night);
LV_IMG_DECLARE(image_mostly_cloudy_day);
LV_IMG_DECLARE(image_mostly_cloudy_night);
LV_IMG_DECLARE(image_mostly_sunny);
LV_IMG_DECLARE(image_partly_cloudy);
LV_IMG_DECLARE(image_partly_cloudy_night);
LV_IMG_DECLARE(image_scattered_showers_day);
LV_IMG_DECLARE(image_scattered_showers_night);
LV_IMG_DECLARE(image_showers_rain);
LV_IMG_DECLARE(image_sleet_hail);
LV_IMG_DECLARE(image_snow_showers_snow);
LV_IMG_DECLARE(image_strong_tstorms);
LV_IMG_DECLARE(image_sunny);
LV_IMG_DECLARE(image_tornado);
LV_IMG_DECLARE(image_wintry_mix_rain_snow);



const lv_img_dsc_t* choose_image(int code, int is_day) {
  switch (code) {
    // Clear sky
    case  0:
      return is_day
        ? &image_sunny
        : &image_clear_night;

    // Mainly clear
    case  1:
      return is_day
        ? &image_mostly_sunny
        : &image_mostly_clear_night;

    // Partly cloudy
    case  2:
      return is_day
        ? &image_partly_cloudy
        : &image_partly_cloudy_night;

    // Overcast
    case  3:
      return &image_cloudy;

    // Fog / mist
    case 45:
    case 48:
      return &image_haze_fog_dust_smoke;

    // Drizzle (light → dense)
    case 51:
    case 53:
    case 55:
      return &image_drizzle;

    // Freezing drizzle
    case 56:
    case 57:
      return &image_sleet_hail;

    // Rain: slight showers
    case 61:
      return is_day
        ? &image_scattered_showers_day
        : &image_scattered_showers_night;

    // Rain: moderate
    case 63:
      return &image_showers_rain;

    // Rain: heavy
    case 65:
      return &image_heavy_rain;

    // Freezing rain
    case 66:
    case 67:
      return &image_wintry_mix_rain_snow;

    // Snow fall (light, moderate, heavy) & snow showers (light)
    case 71:
    case 73:
    case 75:
    case 85:
      return &image_snow_showers_snow;

    // Snow grains
    case 77:
      return &image_flurries;

    // Rain showers (slight → moderate)
    case 80:
    case 81:
      return is_day
        ? &image_scattered_showers_day
        : &image_scattered_showers_night;

    // Rain showers: violent
    case 82:
      return &image_heavy_rain;

    // Heavy snow showers
    case 86:
      return &image_heavy_snow;

    // Thunderstorm (light)
    case 95:
      return is_day
        ? &image_isolated_scattered_tstorms_day
        : &image_isolated_scattered_tstorms_night;

    // Thunderstorm with hail
    case 96:
    case 99:
      return &image_strong_tstorms;

    // Fallback for any other code
    default:
      return is_day
        ? &image_mostly_cloudy_day
        : &image_mostly_cloudy_night;
  }
}

const lv_img_dsc_t* choose_icon(int code, int is_day) {
  switch (code) {
    // Clear sky
    case  0:
      return is_day
        ? &icon_sunny
        : &icon_clear_night;

    // Mainly clear
    case  1:
      return is_day
        ? &icon_mostly_sunny
        : &icon_mostly_clear_night;

    // Partly cloudy
    case  2:
      return is_day
        ? &icon_partly_cloudy
        : &icon_partly_cloudy_night;

    // Overcast
    case  3:
      return &icon_cloudy;

    // Fog / mist
    case 45:
    case 48:
      return &icon_haze_fog_dust_smoke;

    // Drizzle (light → dense)
    case 51:
    case 53:
    case 55:
      return &icon_drizzle;

    // Freezing drizzle
    case 56:
    case 57:
      return &icon_sleet_hail;

    // Rain: slight showers
    case 61:
      return is_day
        ? &icon_scattered_showers_day
        : &icon_scattered_showers_night;

    // Rain: moderate
    case 63:
      return &icon_showers_rain;

    // Rain: heavy
    case 65:
      return &icon_heavy_rain;

    // Freezing rain
    case 66:
    case 67:
      return &icon_wintry_mix_rain_snow;

    // Snow fall (light, moderate, heavy) & snow showers (light)
    case 71:
    case 73:
    case 75:
    case 85:
      return &icon_snow_showers_snow;

    // Snow grains
    case 77:
      return &icon_flurries;

    // Rain showers (slight → moderate)
    case 80:
    case 81:
      return is_day
        ? &icon_scattered_showers_day
        : &icon_scattered_showers_night;

    // Rain showers: violent
    case 82:
      return &icon_heavy_rain;

    // Heavy snow showers
    case 86:
      return &icon_heavy_snow;

    // Thunderstorm (light)
    case 95:
      return is_day
        ? &icon_isolated_scattered_tstorms_day
        : &icon_isolated_scattered_tstorms_night;

    // Thunderstorm with hail
    case 96:
    case 99:
      return &icon_strong_tstorms;

    // Fallback for any other code
    default:
      return is_day
        ? &icon_mostly_cloudy_day
        : &icon_mostly_cloudy_night;
  }
}

#endif //  DECLARATIONS_H