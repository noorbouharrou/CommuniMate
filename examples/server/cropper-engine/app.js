const express = require('express');
const multer = require('multer');
const sharp = require('sharp');
const path = require('path');
const fs = require('fs');
const cors = require('cors');

const app = express();
const PORT = 3000;

app.use(cors());

// Stel de opslag in voor Multer
const storage = multer.memoryStorage(); // Sla bestanden tijdelijk in geheugen op
const upload = multer({ storage: storage });

// Endpoint voor het uploaden en resizen van afbeeldingen
app.post('/resize', upload.single('image'), (req, res) => {
  if (!req.file) {
    return res.status(400).send('No file uploaded.');
  }

  const { width, height } = req.body; // Verkrijg de gewenste breedte en hoogte van de request

  // Zorg ervoor dat breedte en hoogte nummers zijn
  const newWidth = parseInt(width, 10);
  const newHeight = parseInt(height, 10);

  if (isNaN(newWidth) || isNaN(newHeight)) {
    return res.status(400).send('Invalid width or height.');
  }

  // Resize de afbeelding met Sharp
  sharp(req.file.buffer)
  .resize(newWidth, newHeight)
  .toFormat('png') // Specify PNG format
  .toBuffer()
  .then((data) => {
    res.set('Content-Type', 'image/png'); // Set the content type to PNG
    res.send(data);
  })
  .catch((err) => {
    console.error(err);
    res.status(500).send('Error resizing image.');
  });
});

// Start de server
app.listen(PORT, () => {
  console.log(`Server draait op http://localhost:${PORT}`);
});