const express = require('express');
const gtts = require('gtts');
const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');
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
  const { text, language = 'nl' } = req.body; // Taalcode 'nl' voor Nederlands, standaard is Nederlands

  if (!text) {
    return res.status(400).json({ error: 'Text is required.' });
  }

  // Bestandspaden voor MP3 en WAV
  const mp3FilePath = path.join(__dirname, 'output.mp3');
  const wavFilePath = path.join(__dirname, 'output.wav');

  // Maak een nieuwe gtts instantie
  const speech = new gtts(text, language);

  // Zet de tekst om naar MP3 en sla deze op
  speech.save(mp3FilePath, (err) => {
    if (err) {
      console.error('Fout bij genereren van spraak:', err);
      return res.status(500).json({ error: 'Er is iets fout gegaan bij het genereren van spraak.' });
    }

    // Gebruik ffmpeg om MP3 naar WAV te converteren
    exec(`ffmpeg -i "${mp3FilePath}" "${wavFilePath}"`, (ffmpegErr) => {
      // Verwijder het MP3-bestand na conversie
      fs.unlinkSync(mp3FilePath);

      if (ffmpegErr) {
        console.error('Fout bij converteren naar WAV:', ffmpegErr);
        return res.status(500).json({ error: 'Er is iets fout gegaan bij het converteren naar WAV.' });
      }

      // Stuur het WAV-bestand als response
      res.download(wavFilePath, 'output.wav', (downloadErr) => {
        if (downloadErr) {
          console.error('Fout bij verzenden van bestand:', downloadErr);
        }
        // Verwijder het WAV-bestand na verzending
        fs.unlinkSync(wavFilePath);
      });
    });
  });
});

// Start de server
app.listen(PORT, () => {
  console.log(`Server draait op http://localhost:${PORT}`);
});
