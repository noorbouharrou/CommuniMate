const express = require('express');
const gtts = require('gtts');
const fs = require('fs');
const path = require('path');
const bodyParser = require('body-parser');
const cors = require('cors');

const app = express();
const PORT = 3000;

// Stel JSON parsing in
app.use(bodyParser.json());

// Enable CORS for all origins (allowing cross-origin requests)
app.use(cors());

// Endpoint voor tekst naar spraak
app.post('/tts', (req, res) => {
  const { text, language = 'nl' } = req.body;  // Taalcode 'nl' voor Nederlands, standaard is Nederlands

  if (!text) {
    return res.status(400).json({ error: 'Text is required.' });
  }

  // Maak een nieuwe gtts instantie
  const speech = new gtts(text, language);
  
  // Bestandspad voor het MP3-bestand
  const filePath = path.join(__dirname, 'output.mp3');
  
  // Zet de tekst om naar MP3 en sla deze op
  speech.save(filePath, (err, result) => {
    if (err) {
      console.error('Fout bij genereren van spraak:', err);
      return res.status(500).json({ error: 'Er is iets fout gegaan bij het genereren van spraak.' });
    }

    // Stuur het MP3-bestand als response
    res.download(filePath, 'output.mp3', (err) => {
      if (err) {
        console.error('Fout bij verzenden van bestand:', err);
      }
      // Verwijder het bestand na verzending
      fs.unlinkSync(filePath);
    });
  });
});

// Start de server
app.listen(PORT, () => {
  console.log(`Server draait op http://localhost:${PORT}`);
});
