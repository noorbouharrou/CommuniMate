const express = require('express');
const multer = require('multer');
const sharp = require('sharp');
const cors = require('cors');

const app = express();
const PORT = 3000;

app.use(cors());

const storage = multer.memoryStorage();
const upload = multer({ storage: storage });

app.post('/resize', upload.single('image'), async (req, res) => {
  if (!req.file) {
    return res.status(400).send('No file uploaded.');
  }

  const { width, height, label } = req.body;
  const newWidth = parseInt(width, 10);
  const newHeight = parseInt(height, 10);

  if (isNaN(newWidth) || isNaN(newHeight)) {
    return res.status(400).send('Invalid width or height.');
  }

  // Convert label to uppercase
  const uppercaseLabel = label.toUpperCase();

  // SVG with uppercase yellow text
  const svg = `
  <svg width="${newWidth}" height="${newHeight}">
    <rect x="0%" y="72%" width="100%" height="25%" fill="rgba(255,255,255,0.8)" rx="10" />
    <text x="50%" y="90%" text-anchor="middle" fill="#000000" font-size="32" font-family="Arial" font-weight="bold">${uppercaseLabel}</text>
  </svg>
  `;

  const svgBuffer = Buffer.from(svg);

  try {
    const resizedImage = await sharp(req.file.buffer)
      .resize(newWidth, newHeight)
      .composite([{ input: svgBuffer, top: 0, left: 0 }])
      .toBuffer();

    res.set('Content-Type', 'image/png');
    res.send(resizedImage);
  } catch (err) {
    console.error(err);
    res.status(500).send('Error processing image.');
  }
});

app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});

