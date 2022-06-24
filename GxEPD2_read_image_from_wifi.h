//void showBitmapFrom_HTTP(const char* host, const char* path, const char* filename, int16_t x, int16_t y, bool with_color)
//{
//  WiFiClient client;
//  bool connection_ok = false;
//  bool valid = false; // valid format to be handled
//  bool flip = true; // bitmap is stored bottom-to-top
//  uint32_t startTime = millis();
//  if ((x >= display.epd2.WIDTH) || (y >= display.epd2.HEIGHT)) return;
//  Serial.println(); Serial.print("downloading file \""); Serial.print(filename);  Serial.println("\"");
//  Serial.print("connecting to "); Serial.println(host);
//  if (!client.connect(host, httpPort))
//  {
//    Serial.println("connection failed");
//    return;
//  }
//  Serial.print("requesting URL: ");
//  Serial.println(String("http://") + host + path + filename);
//  client.print(String("GET ") + path + filename + " HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" +
//               "User-Agent: GxEPD2_WiFi_Example\r\n" +
//               "Connection: close\r\n\r\n");
//  Serial.println("request sent");
//  while (client.connected())
//  {
//    String line = client.readStringUntil('\n');
//    if (!connection_ok)
//    {
//      connection_ok = line.startsWith("HTTP/1.1 200 OK");
//      if (connection_ok) Serial.println(line);
//      //if (!connection_ok) Serial.println(line);
//    }
//    if (!connection_ok) Serial.println(line);
//    //Serial.println(line);
//    if (line == "\r")
//    {
//      Serial.println("headers received");
//      break;
//    }
//  }
//  if (!connection_ok) return;
//  // Parse BMP header
//  if (read16(client) == 0x4D42) // BMP signature
//  {
//    uint32_t fileSize = read32(client);
//    uint32_t creatorBytes = read32(client);
//    uint32_t imageOffset = read32(client); // Start of image data
//    uint32_t headerSize = read32(client);
//    uint32_t width  = read32(client);
//    uint32_t height = read32(client);
//    uint16_t planes = read16(client);
//    uint16_t depth = read16(client); // bits per pixel
//    uint32_t format = read32(client);
//    uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
//    if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
//    {
//      Serial.print("File size: "); Serial.println(fileSize);
//      Serial.print("Image Offset: "); Serial.println(imageOffset);
//      Serial.print("Header size: "); Serial.println(headerSize);
//      Serial.print("Bit Depth: "); Serial.println(depth);
//      Serial.print("Image size: ");
//      Serial.print(width);
//      Serial.print('x');
//      Serial.println(height);
//      // BMP rows are padded (if needed) to 4-byte boundary
//      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
//      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
//      if (height < 0)
//      {
//        height = -height;
//        flip = false;
//      }
//      uint16_t w = width;
//      uint16_t h = height;
//      if ((x + w - 1) >= display.epd2.WIDTH)  w = display.epd2.WIDTH  - x;
//      if ((y + h - 1) >= display.epd2.HEIGHT) h = display.epd2.HEIGHT - y;
//      if (w <= max_row_width) // handle with direct drawing
//      {
//        valid = true;
//        uint8_t bitmask = 0xFF;
//        uint8_t bitshift = 8 - depth;
//        uint16_t red, green, blue;
//        bool whitish, colored;
//        if (depth == 1) with_color = false;
//        if (depth <= 8)
//        {
//          if (depth < 8) bitmask >>= depth;
//          //bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
//          bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
//          for (uint16_t pn = 0; pn < (1 << depth); pn++)
//          {
//            blue  = client.read();
//            green = client.read();
//            red   = client.read();
//            client.read();
//            bytes_read += 4;
//            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
//            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
//            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
//            color_palette_buffer[pn / 8] |= colored << pn % 8;
//          }
//        }
//        display.clearScreen();
//        uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
//        //Serial.print("skip "); Serial.println(rowPosition - bytes_read);
//        bytes_read += skip(client, rowPosition - bytes_read);
//        for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
//        {
//          if (!connection_ok || !(client.connected() || client.available())) break;
//          delay(1); // yield() to avoid WDT
//          uint32_t in_remain = rowSize;
//          uint32_t in_idx = 0;
//          uint32_t in_bytes = 0;
//          uint8_t in_byte = 0; // for depth <= 8
//          uint8_t in_bits = 0; // for depth <= 8
//          uint8_t out_byte = 0xFF; // white (for w%8!=0 border)
//          uint8_t out_color_byte = 0xFF; // white (for w%8!=0 border)
//          uint32_t out_idx = 0;
//          for (uint16_t col = 0; col < w; col++) // for each pixel
//          {
//            yield();
//            if (!connection_ok || !(client.connected() || client.available())) break;
//            // Time to read more pixel data?
//            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
//            {
//              uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
//              uint32_t got = read8n(client, input_buffer, get);
//              while ((got < get) && connection_ok)
//              {
//                //Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
//                uint32_t gotmore = read8n(client, input_buffer + got, get - got);
//                got += gotmore;
//                connection_ok = gotmore > 0;
//              }
//              in_bytes = got;
//              in_remain -= got;
//              bytes_read += got;
//            }
//            if (!connection_ok)
//            {
//              Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
//              break;
//            }
//            switch (depth)
//            {
//              case 24:
//                blue = input_buffer[in_idx++];
//                green = input_buffer[in_idx++];
//                red = input_buffer[in_idx++];
//                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                break;
//              case 16:
//                {
//                  uint8_t lsb = input_buffer[in_idx++];
//                  uint8_t msb = input_buffer[in_idx++];
//                  if (format == 0) // 555
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
//                    red   = (msb & 0x7C) << 1;
//                  }
//                  else // 565
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
//                    red   = (msb & 0xF8);
//                  }
//                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                }
//                break;
//              case 1:
//              case 4:
//              case 8:
//                {
//                  if (0 == in_bits)
//                  {
//                    in_byte = input_buffer[in_idx++];
//                    in_bits = 8;
//                  }
//                  uint16_t pn = (in_byte >> bitshift) & bitmask;
//                  whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  in_byte <<= depth;
//                  in_bits -= depth;
//                }
//                break;
//            }
//            if (whitish)
//            {
//              // keep white
//            }
//            else if (colored && with_color)
//            {
//              out_color_byte &= ~(0x80 >> col % 8); // colored
//            }
//            else
//            {
//              out_byte &= ~(0x80 >> col % 8); // black
//            }
//            if ((7 == col % 8) || (col == w - 1)) // write that last byte! (for w%8!=0 border)
//            {
//              output_row_color_buffer[out_idx] = out_color_byte;
//              output_row_mono_buffer[out_idx++] = out_byte;
//              out_byte = 0xFF; // white (for w%8!=0 border)
//              out_color_byte = 0xFF; // white (for w%8!=0 border)
//            }
//          } // end pixel
//          int16_t yrow = y + (flip ? h - row - 1 : row);
//          display.writeImage(output_row_mono_buffer, output_row_color_buffer, x, yrow, w, 1);
//        } // end line
//        Serial.print("downloaded in ");
//        Serial.print(millis() - startTime);
//        Serial.println(" ms");
//        display.refresh();
//      }
//      Serial.print("bytes read "); Serial.println(bytes_read);
//    }
//  }
//  client.stop();
//  if (!valid)
//  {
//    Serial.println("bitmap format not handled.");
//  }
//}
//
//void drawBitmapFrom_HTTP_ToBuffer(const char* host, const char* path, const char* filename, int16_t x, int16_t y, bool with_color)
//{
//  WiFiClient client;
//  bool connection_ok = false;
//  bool valid = false; // valid format to be handled
//  bool flip = true; // bitmap is stored bottom-to-top
//  bool has_multicolors = display.epd2.panel == GxEPD2::ACeP565;
//  uint32_t startTime = millis();
//  if ((x >= display.width()) || (y >= display.height())) return;
//  display.fillScreen(GxEPD_WHITE);
//  Serial.print("connecting to "); Serial.println(host);
//  if (!client.connect(host, httpPort))
//  {
//    Serial.println("connection failed");
//    return;
//  }
//  Serial.print("requesting URL: ");
//  Serial.println(String("http://") + host + path + filename);
//  client.print(String("GET ") + path + filename + " HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" +
//               "User-Agent: GxEPD2_WiFi_Example\r\n" +
//               "Connection: close\r\n\r\n");
//  Serial.println("request sent");
//  while (client.connected())
//  {
//    String line = client.readStringUntil('\n');
//    if (!connection_ok)
//    {
//      connection_ok = line.startsWith("HTTP/1.1 200 OK");
//      if (connection_ok) Serial.println(line);
//      //if (!connection_ok) Serial.println(line);
//    }
//    if (!connection_ok) Serial.println(line);
//    //Serial.println(line);
//    if (line == "\r")
//    {
//      Serial.println("headers received");
//      break;
//    }
//  }
//  if (!connection_ok) return;
//  // Parse BMP header
//  if (read16(client) == 0x4D42) // BMP signature
//  {
//    uint32_t fileSize = read32(client);
//    uint32_t creatorBytes = read32(client);
//    uint32_t imageOffset = read32(client); // Start of image data
//    uint32_t headerSize = read32(client);
//    uint32_t width  = read32(client);
//    uint32_t height = read32(client);
//    uint16_t planes = read16(client);
//    uint16_t depth = read16(client); // bits per pixel
//    uint32_t format = read32(client);
//    uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
//    if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
//    {
//      Serial.print("File size: "); Serial.println(fileSize);
//      Serial.print("Image Offset: "); Serial.println(imageOffset);
//      Serial.print("Header size: "); Serial.println(headerSize);
//      Serial.print("Bit Depth: "); Serial.println(depth);
//      Serial.print("Image size: ");
//      Serial.print(width);
//      Serial.print('x');
//      Serial.println(height);
//      // BMP rows are padded (if needed) to 4-byte boundary
//      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
//      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
//      if (height < 0)
//      {
//        height = -height;
//        flip = false;
//      }
//      uint16_t w = width;
//      uint16_t h = height;
//      if ((x + w - 1) >= display.width())  w = display.width()  - x;
//      if ((y + h - 1) >= display.height()) h = display.height() - y;
//      //if (w <= max_row_width) // handle with direct drawing
//      {
//        valid = true;
//        uint8_t bitmask = 0xFF;
//        uint8_t bitshift = 8 - depth;
//        uint16_t red, green, blue;
//        bool whitish, colored;
//        if (depth == 1) with_color = false;
//        if (depth <= 8)
//        {
//          if (depth < 8) bitmask >>= depth;
//          //bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
//          bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
//          for (uint16_t pn = 0; pn < (1 << depth); pn++)
//          {
//            blue  = client.read();
//            green = client.read();
//            red   = client.read();
//            client.read();
//            bytes_read += 4;
//            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
//            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
//            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
//            color_palette_buffer[pn / 8] |= colored << pn % 8;
//            //Serial.print("0x00"); Serial.print(red, HEX); Serial.print(green, HEX); Serial.print(blue, HEX);
//            //Serial.print(" : "); Serial.print(whitish); Serial.print(", "); Serial.println(colored);
//            rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
//          }
//        }
//        uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
//        //Serial.print("skip "); Serial.println(rowPosition - bytes_read);
//        bytes_read += skip(client, rowPosition - bytes_read);
//        for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
//        {
//          if (!connection_ok || !(client.connected() || client.available())) break;
//          delay(1); // yield() to avoid WDT
//          uint32_t in_remain = rowSize;
//          uint32_t in_idx = 0;
//          uint32_t in_bytes = 0;
//          uint8_t in_byte = 0; // for depth <= 8
//          uint8_t in_bits = 0; // for depth <= 8
//          uint16_t color = GxEPD_WHITE;
//          for (uint16_t col = 0; col < w; col++) // for each pixel
//          {
//            yield();
//            if (!connection_ok || !(client.connected() || client.available())) break;
//            // Time to read more pixel data?
//            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
//            {
//              uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
//              uint32_t got = read8n(client, input_buffer, get);
//              while ((got < get) && connection_ok)
//              {
//                //Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
//                uint32_t gotmore = read8n(client, input_buffer + got, get - got);
//                got += gotmore;
//                connection_ok = gotmore > 0;
//              }
//              in_bytes = got;
//              in_remain -= got;
//              bytes_read += got;
//            }
//            if (!connection_ok)
//            {
//              Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
//              break;
//            }
//            switch (depth)
//            {
//              case 24:
//                blue = input_buffer[in_idx++];
//                green = input_buffer[in_idx++];
//                red = input_buffer[in_idx++];
//                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
//                break;
//              case 16:
//                {
//                  uint8_t lsb = input_buffer[in_idx++];
//                  uint8_t msb = input_buffer[in_idx++];
//                  if (format == 0) // 555
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
//                    red   = (msb & 0x7C) << 1;
//                    color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
//                  }
//                  else // 565
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
//                    red   = (msb & 0xF8);
//                    color = (msb << 8) | lsb;
//                  }
//                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                }
//                break;
//              case 1:
//              case 4:
//              case 8:
//                {
//                  if (0 == in_bits)
//                  {
//                    in_byte = input_buffer[in_idx++];
//                    in_bits = 8;
//                  }
//                  uint16_t pn = (in_byte >> bitshift) & bitmask;
//                  whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  in_byte <<= depth;
//                  in_bits -= depth;
//                  color = rgb_palette_buffer[pn];
//                }
//                break;
//            }
//            if (with_color && has_multicolors)
//            {
//              // keep color
//            }
//            else if (whitish)
//            {
//              color = GxEPD_WHITE;
//            }
//            else if (colored && with_color)
//            {
//              color = GxEPD_COLORED;
//            }
//            else
//            {
//              color = GxEPD_BLACK;
//            }
//            uint16_t yrow = y + (flip ? h - row - 1 : row);
//            display.drawPixel(x + col, yrow, color);
//          } // end pixel
//        } // end line
//      }
//      Serial.print("bytes read "); Serial.println(bytes_read);
//    }
//  }
//  Serial.print("loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
//  client.stop();
//  if (!valid)
//  {
//    Serial.println("bitmap format not handled.");
//  }
//}
//
//void showBitmapFrom_HTTP_Buffered(const char* host, const char* path, const char* filename, int16_t x, int16_t y, bool with_color)
//{
//  Serial.println(); Serial.print("downloading file \""); Serial.print(filename);  Serial.println("\"");
//  display.setFullWindow();
//  display.firstPage();
//  do
//  {
//    drawBitmapFrom_HTTP_ToBuffer(host, path, filename, x, y, with_color);
//  }
//  while (display.nextPage());
//}
//
//void showBitmapFrom_HTTPS(const char* host, const char* path, const char* filename, const char* fingerprint, int16_t x, int16_t y, bool with_color, const char* certificate)
//{
//  // Use WiFiClientSecure class to create TLS connection
//#if USE_BearSSL
//  BearSSL::WiFiClientSecure client;
//#else
//  WiFiClientSecure client;
//#endif
//  bool connection_ok = false;
//  bool valid = false; // valid format to be handled
//  bool flip = true; // bitmap is stored bottom-to-top
//  uint32_t startTime = millis();
//  if ((x >= display.epd2.WIDTH) || (y >= display.epd2.HEIGHT)) return;
//  Serial.println(); Serial.print("downloading file \""); Serial.print(filename);  Serial.println("\"");
//  Serial.print("connecting to "); Serial.println(host);
//#if defined (ESP8266)
//  client.setBufferSizes(4096, 4096); // doesn't help
//  if (fingerprint) client.setFingerprint(fingerprint);
//#elif defined (ESP32)
//  if (certificate) client.setCACert(certificate);
//#endif
//  if (!client.connect(host, httpsPort))
//  {
//    Serial.println("connection failed");
//    return;
//  }
//  Serial.print("requesting URL: ");
//  Serial.println(String("https://") + host + path + filename);
//  client.print(String("GET ") + path + filename + " HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" +
//               "User-Agent: GxEPD2_WiFi_Example\r\n" +
//               "Connection: close\r\n\r\n");
//  Serial.println("request sent");
//  while (client.connected())
//  {
//    String line = client.readStringUntil('\n');
//    if (!connection_ok)
//    {
//      connection_ok = line.startsWith("HTTP/1.1 200 OK");
//      if (connection_ok) Serial.println(line);
//      //if (!connection_ok) Serial.println(line);
//    }
//    if (!connection_ok) Serial.println(line);
//    //Serial.println(line);
//    if (line == "\r")
//    {
//      Serial.println("headers received");
//      break;
//    }
//  }
//  if (!connection_ok) return;
//  // Parse BMP header
//  if (read16(client) == 0x4D42) // BMP signature
//  {
//    uint32_t fileSize = read32(client);
//    uint32_t creatorBytes = read32(client);
//    uint32_t imageOffset = read32(client); // Start of image data
//    uint32_t headerSize = read32(client);
//    uint32_t width  = read32(client);
//    uint32_t height = read32(client);
//    uint16_t planes = read16(client);
//    uint16_t depth = read16(client); // bits per pixel
//    uint32_t format = read32(client);
//    uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
//    if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
//    {
//      Serial.print("File size: "); Serial.println(fileSize);
//      Serial.print("Image Offset: "); Serial.println(imageOffset);
//      Serial.print("Header size: "); Serial.println(headerSize);
//      Serial.print("Bit Depth: "); Serial.println(depth);
//      Serial.print("Image size: ");
//      Serial.print(width);
//      Serial.print('x');
//      Serial.println(height);
//      // BMP rows are padded (if needed) to 4-byte boundary
//      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
//      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
//      if (height < 0)
//      {
//        height = -height;
//        flip = false;
//      }
//      uint16_t w = width;
//      uint16_t h = height;
//      if ((x + w - 1) >= display.epd2.WIDTH)  w = display.epd2.WIDTH  - x;
//      if ((y + h - 1) >= display.epd2.HEIGHT) h = display.epd2.HEIGHT - y;
//      if (w <= max_row_width) // handle with direct drawing
//      {
//        valid = true;
//        uint8_t bitmask = 0xFF;
//        uint8_t bitshift = 8 - depth;
//        uint16_t red, green, blue;
//        bool whitish, colored;
//        if (depth == 1) with_color = false;
//        if (depth <= 8)
//        {
//          if (depth < 8) bitmask >>= depth;
//          //bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
//          bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
//          for (uint16_t pn = 0; pn < (1 << depth); pn++)
//          {
//            blue  = client.read();
//            green = client.read();
//            red   = client.read();
//            client.read();
//            bytes_read += 4;
//            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
//            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
//            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
//            color_palette_buffer[pn / 8] |= colored << pn % 8;
//          }
//        }
//        display.clearScreen();
//        uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
//        //Serial.print("skip "); Serial.println(rowPosition - bytes_read);
//        bytes_read += skip(client, rowPosition - bytes_read);
//        for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
//        {
//          if (!connection_ok || !(client.connected() || client.available())) break;
//          delay(1); // yield() to avoid WDT
//          uint32_t in_remain = rowSize;
//          uint32_t in_idx = 0;
//          uint32_t in_bytes = 0;
//          uint8_t in_byte = 0; // for depth <= 8
//          uint8_t in_bits = 0; // for depth <= 8
//          uint8_t out_byte = 0xFF; // white (for w%8!=0 border)
//          uint8_t out_color_byte = 0xFF; // white (for w%8!=0 border)
//          uint32_t out_idx = 0;
//          for (uint16_t col = 0; col < w; col++) // for each pixel
//          {
//            yield();
//            if (!connection_ok || !(client.connected() || client.available())) break;
//            // Time to read more pixel data?
//            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
//            {
//              uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
//              uint32_t got = read8n(client, input_buffer, get);
//              while ((got < get) && connection_ok)
//              {
//                //Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
//                //if ((get - got) > client.available()) delay(200); // does improve? yes, if >= 200
//                uint32_t gotmore = read8n(client, input_buffer + got, get - got);
//                got += gotmore;
//                connection_ok = gotmore > 0;
//              }
//              in_bytes = got;
//              in_remain -= got;
//              bytes_read += got;
//            }
//            if (!connection_ok)
//            {
//              Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
//              break;
//            }
//            switch (depth)
//            {
//              case 24:
//                blue = input_buffer[in_idx++];
//                green = input_buffer[in_idx++];
//                red = input_buffer[in_idx++];
//                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                break;
//              case 16:
//                {
//                  uint8_t lsb = input_buffer[in_idx++];
//                  uint8_t msb = input_buffer[in_idx++];
//                  if (format == 0) // 555
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
//                    red   = (msb & 0x7C) << 1;
//                  }
//                  else // 565
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
//                    red   = (msb & 0xF8);
//                  }
//                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                }
//                break;
//              case 1:
//              case 4:
//              case 8:
//                {
//                  if (0 == in_bits)
//                  {
//                    in_byte = input_buffer[in_idx++];
//                    in_bits = 8;
//                  }
//                  uint16_t pn = (in_byte >> bitshift) & bitmask;
//                  whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  in_byte <<= depth;
//                  in_bits -= depth;
//                }
//                break;
//            }
//            if (whitish)
//            {
//              // keep white
//            }
//            else if (colored && with_color)
//            {
//              out_color_byte &= ~(0x80 >> col % 8); // colored
//            }
//            else
//            {
//              out_byte &= ~(0x80 >> col % 8); // black
//            }
//            if ((7 == col % 8) || (col == w - 1)) // write that last byte! (for w%8!=0 border)
//            {
//              output_row_color_buffer[out_idx] = out_color_byte;
//              output_row_mono_buffer[out_idx++] = out_byte;
//              out_byte = 0xFF; // white (for w%8!=0 border)
//              out_color_byte = 0xFF; // white (for w%8!=0 border)
//            }
//          } // end pixel
//          int16_t yrow = y + (flip ? h - row - 1 : row);
//          display.writeImage(output_row_mono_buffer, output_row_color_buffer, x, yrow, w, 1);
//        } // end line
//        Serial.print("downloaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
//        Serial.print("bytes read "); Serial.println(bytes_read);
//        display.refresh();
//      }
//    }
//  }
//  client.stop();
//  if (!valid)
//  {
//    Serial.println("bitmap format not handled.");
//  }
//}
//
//void drawBitmapFrom_HTTPS_ToBuffer(const char* host, const char* path, const char* filename, const char* fingerprint, int16_t x, int16_t y, bool with_color, const char* certificate)
//{
//  // Use WiFiClientSecure class to create TLS connection
//#if USE_BearSSL
//  BearSSL::WiFiClientSecure client;
//#else
//  WiFiClientSecure client;
//#endif
//  bool connection_ok = false;
//  bool valid = false; // valid format to be handled
//  bool flip = true; // bitmap is stored bottom-to-top
//  bool has_multicolors = display.epd2.panel == GxEPD2::ACeP565;
//  uint32_t startTime = millis();
//  if ((x >= display.width()) || (y >= display.height())) return;
//  display.fillScreen(GxEPD_WHITE);
//  Serial.print("connecting to "); Serial.println(host);
//#if defined (ESP8266)
//  client.setBufferSizes(4096, 4096); // doesn't help
//  if (fingerprint) client.setFingerprint(fingerprint);
//#elif defined (ESP32)
//  if (certificate) client.setCACert(certificate);
//#endif
//  if (!client.connect(host, httpsPort))
//  {
//    Serial.println("connection failed");
//    return;
//  }
//  Serial.print("requesting URL: ");
//  Serial.println(String("https://") + host + path + filename);
//  client.print(String("GET ") + path + filename + " HTTP/1.1\r\n" +
//               "Host: " + host + "\r\n" +
//               "User-Agent: GxEPD2_WiFi_Example\r\n" +
//               "Connection: close\r\n\r\n");
//  Serial.println("request sent");
//  while (client.connected())
//  {
//    String line = client.readStringUntil('\n');
//    if (!connection_ok)
//    {
//      connection_ok = line.startsWith("HTTP/1.1 200 OK");
//      if (connection_ok) Serial.println(line);
//      //if (!connection_ok) Serial.println(line);
//    }
//    if (!connection_ok) Serial.println(line);
//    //Serial.println(line);
//    if (line == "\r")
//    {
//      Serial.println("headers received");
//      break;
//    }
//  }
//  if (!connection_ok) return;
//  // Parse BMP header
//  if (read16(client) == 0x4D42) // BMP signature
//  {
//    uint32_t fileSize = read32(client);
//    uint32_t creatorBytes = read32(client);
//    uint32_t imageOffset = read32(client); // Start of image data
//    uint32_t headerSize = read32(client);
//    uint32_t width  = read32(client);
//    uint32_t height = read32(client);
//    uint16_t planes = read16(client);
//    uint16_t depth = read16(client); // bits per pixel
//    uint32_t format = read32(client);
//    uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
//    if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
//    {
//      Serial.print("File size: "); Serial.println(fileSize);
//      Serial.print("Image Offset: "); Serial.println(imageOffset);
//      Serial.print("Header size: "); Serial.println(headerSize);
//      Serial.print("Bit Depth: "); Serial.println(depth);
//      Serial.print("Image size: ");
//      Serial.print(width);
//      Serial.print('x');
//      Serial.println(height);
//      // BMP rows are padded (if needed) to 4-byte boundary
//      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
//      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
//      if (height < 0)
//      {
//        height = -height;
//        flip = false;
//      }
//      uint16_t w = width;
//      uint16_t h = height;
//      if ((x + w - 1) >= display.width())  w = display.width()  - x;
//      if ((y + h - 1) >= display.height()) h = display.height() - y;
//      //if (w <= max_row_width) // handle with direct drawing
//      {
//        valid = true;
//        uint8_t bitmask = 0xFF;
//        uint8_t bitshift = 8 - depth;
//        uint16_t red, green, blue;
//        bool whitish, colored;
//        if (depth == 1) with_color = false;
//        if (depth <= 8)
//        {
//          if (depth < 8) bitmask >>= depth;
//          //bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
//          bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
//          for (uint16_t pn = 0; pn < (1 << depth); pn++)
//          {
//            blue  = client.read();
//            green = client.read();
//            red   = client.read();
//            client.read();
//            bytes_read += 4;
//            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
//            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
//            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
//            color_palette_buffer[pn / 8] |= colored << pn % 8;
//            //Serial.print("0x00"); Serial.print(red, HEX); Serial.print(green, HEX); Serial.print(blue, HEX);
//            //Serial.print(" : "); Serial.print(whitish); Serial.print(", "); Serial.println(colored);
//            rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
//          }
//        }
//        uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
//        //Serial.print("skip "); Serial.println(rowPosition - bytes_read);
//        bytes_read += skip(client, rowPosition - bytes_read);
//        for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
//        {
//          if (!connection_ok || !(client.connected() || client.available())) break;
//          delay(1); // yield() to avoid WDT
//          uint32_t in_remain = rowSize;
//          uint32_t in_idx = 0;
//          uint32_t in_bytes = 0;
//          uint8_t in_byte = 0; // for depth <= 8
//          uint8_t in_bits = 0; // for depth <= 8
//          uint16_t color = GxEPD_WHITE;
//          for (uint16_t col = 0; col < w; col++) // for each pixel
//          {
//            yield();
//            if (!connection_ok || !(client.connected() || client.available())) break;
//            // Time to read more pixel data?
//            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
//            {
//              uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
//              uint32_t got = read8n(client, input_buffer, get);
//              while ((got < get) && connection_ok)
//              {
//                //Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
//                uint32_t gotmore = read8n(client, input_buffer + got, get - got);
//                got += gotmore;
//                connection_ok = gotmore > 0;
//              }
//              in_bytes = got;
//              in_remain -= got;
//              bytes_read += got;
//            }
//            if (!connection_ok)
//            {
//              Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
//              break;
//            }
//            switch (depth)
//            {
//              case 24:
//                blue = input_buffer[in_idx++];
//                green = input_buffer[in_idx++];
//                red = input_buffer[in_idx++];
//                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
//                break;
//              case 16:
//                {
//                  uint8_t lsb = input_buffer[in_idx++];
//                  uint8_t msb = input_buffer[in_idx++];
//                  if (format == 0) // 555
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
//                    red   = (msb & 0x7C) << 1;
//                    color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
//                  }
//                  else // 565
//                  {
//                    blue  = (lsb & 0x1F) << 3;
//                    green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
//                    red   = (msb & 0xF8);
//                    color = (msb << 8) | lsb;
//                  }
//                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
//                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
//                }
//                break;
//              case 1:
//              case 4:
//              case 8:
//                {
//                  if (0 == in_bits)
//                  {
//                    in_byte = input_buffer[in_idx++];
//                    in_bits = 8;
//                  }
//                  uint16_t pn = (in_byte >> bitshift) & bitmask;
//                  whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
//                  in_byte <<= depth;
//                  in_bits -= depth;
//                  color = rgb_palette_buffer[pn];
//                }
//                break;
//            }
//            if (with_color && has_multicolors)
//            {
//              // keep color
//            }
//            else if (whitish)
//            {
//              color = GxEPD_WHITE;
//            }
//            else if (colored && with_color)
//            {
//              color = GxEPD_COLORED;
//            }
//            else
//            {
//              color = GxEPD_BLACK;
//            }
//            uint16_t yrow = y + (flip ? h - row - 1 : row);
//            display.drawPixel(x + col, yrow, color);
//          } // end pixel
//        } // end line
//      }
//      Serial.print("bytes read "); Serial.println(bytes_read);
//    }
//  }
//  Serial.print("loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
//  client.stop();
//  if (!valid)
//  {
//    Serial.println("bitmap format not handled.");
//  }
//}
//
//void showBitmapFrom_HTTPS_Buffered(const char* host, const char* path, const char* filename, const char* fingerprint, int16_t x, int16_t y, bool with_color, const char* certificate)
//{
//  Serial.println(); Serial.print("downloading file \""); Serial.print(filename);  Serial.println("\"");
//  display.setFullWindow();
//  display.firstPage();
//  do
//  {
//    drawBitmapFrom_HTTPS_ToBuffer(host, path, filename, fingerprint, x, y, with_color, certificate);
//  }
//  while (display.nextPage());
//}
//
//uint16_t read16(WiFiClient& client)
//{
//  // BMP data is stored little-endian, same as Arduino.
//  uint16_t result;
//  ((uint8_t *)&result)[0] = client.read(); // LSB
//  ((uint8_t *)&result)[1] = client.read(); // MSB
//  return result;
//}
//
//uint32_t read32(WiFiClient& client)
//{
//  // BMP data is stored little-endian, same as Arduino.
//  uint32_t result;
//  ((uint8_t *)&result)[0] = client.read(); // LSB
//  ((uint8_t *)&result)[1] = client.read();
//  ((uint8_t *)&result)[2] = client.read();
//  ((uint8_t *)&result)[3] = client.read(); // MSB
//  return result;
//}
//
//#if USE_BearSSL
//
//uint32_t skip(BearSSL::WiFiClientSecure& client, int32_t bytes)
//{
//  int32_t remain = bytes;
//  uint32_t start = millis();
//  while ((client.connected() || client.available()) && (remain > 0))
//  {
//    if (client.available())
//    {
//      int16_t v = client.read();
//      remain--;
//    }
//    else delay(1);
//    if (millis() - start > 2000) break; // don't hang forever
//  }
//  return bytes - remain;
//}
//
//uint32_t read8n(BearSSL::WiFiClientSecure& client, uint8_t* buffer, int32_t bytes)
//{
//  int32_t remain = bytes;
//  uint32_t start = millis();
//  while ((client.connected() || client.available()) && (remain > 0))
//  {
//    if (client.available())
//    {
//      int16_t v = client.read();
//      *buffer++ = uint8_t(v);
//      remain--;
//    }
//    else delay(1);
//    if (millis() - start > 2000) break; // don't hang forever
//  }
//  return bytes - remain;
//}
//
//#endif
//
//uint32_t skip(WiFiClient& client, int32_t bytes)
//{
//  int32_t remain = bytes;
//  uint32_t start = millis();
//  while ((client.connected() || client.available()) && (remain > 0))
//  {
//    if (client.available())
//    {
//      int16_t v = client.read();
//      remain--;
//    }
//    else delay(1);
//    if (millis() - start > 2000) break; // don't hang forever
//  }
//  return bytes - remain;
//}
//
//uint32_t read8n(WiFiClient& client, uint8_t* buffer, int32_t bytes)
//{
//  int32_t remain = bytes;
//  uint32_t start = millis();
//  while ((client.connected() || client.available()) && (remain > 0))
//  {
//    if (client.available())
//    {
//      int16_t v = client.read();
//      *buffer++ = uint8_t(v);
//      remain--;
//    }
//    else delay(1);
//    if (millis() - start > 2000) break; // don't hang forever
//  }
//  return bytes - remain;
//}
