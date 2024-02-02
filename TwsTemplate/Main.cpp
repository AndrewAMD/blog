#include <stdio.h>
#include <memory>
#include "TwsTemplate.h"

std::unique_ptr <TwsTemplate> pTemplate;

int main() {
	printf("Hello world\r\n");
	pTemplate = std::make_unique< TwsTemplate>();
	pTemplate.reset();
	printf("Done!\r\n");
}


