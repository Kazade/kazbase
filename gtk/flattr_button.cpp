
#include "flattr_button.h"

const std::string FLATTR_IMAGE_URL = "https://api.flattr.com/button/view/?url=http%3A%2F%2Fwww.kazade.co.uk%2Fooosh";

FlattrButton::FlattrButton(LockingImageStore* store):
    image_(store) {
    image_.set_image_url(FLATTR_IMAGE_URL);
    button_.set_image(image_);

    add(button_);
}
