let imageBlobs = [], audioBlobs = [];

async function processForm(imageId, textInputId, resizedImageId, audioPlayerId, index) {
  const text = document.getElementById(textInputId).value;
  if (!text) {
    alert(`Vul tekst in voor formulier ${index}!`);
    return;
  }

  const imageFile = document.getElementById(imageId).files[0];
  if (!imageFile) {
    alert(`Selecteer een afbeelding voor formulier ${index}!`);
    return;
  }

  // Process Image
  const imgBlob = await resizeImage(imageFile, 240, 240);
  document.getElementById(resizedImageId).src = URL.createObjectURL(imgBlob);
  imageBlobs[index] = imgBlob;

  // Process Text-to-Speech
  const audioBlob = await generateSpeech(text);
  document.getElementById(audioPlayerId).src = URL.createObjectURL(audioBlob);
  audioBlobs[index] = audioBlob;
}

async function resizeImage(file, width, height) {
  const img = new Image();
  img.src = URL.createObjectURL(file);

  await new Promise((resolve) => (img.onload = resolve));

  const canvas = document.createElement('canvas');
  canvas.width = width;
  canvas.height = height;
  const ctx = canvas.getContext('2d');
  ctx.drawImage(img, 0, 0, width, height);

  return new Promise((resolve) => canvas.toBlob(resolve, 'image/jpeg'));
}

async function generateSpeech(text) {
  const response = await fetch('https://api.streampower.nl/tts', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ text })
  });

  if (!response.ok) throw new Error('Text-to-Speech API error');

  return await response.blob();
}

document.getElementById('processButton').addEventListener('click', async () => {
  for (let i = 1; i <= 8; i++) {
    await processForm(`image${i}`, `textInput${i}`, `resizedImage${i}`, `audioPlayer${i}`, i);
  }
});

document.getElementById('downloadZip').addEventListener('click', () => {
  const zip = new JSZip();

  for (let i = 1; i <= 8; i++) {
    if (imageBlobs[i]) zip.file(`image${i}.jpg`, imageBlobs[i]);
    if (audioBlobs[i]) zip.file(`audio${i}.mp3`, audioBlobs[i]);
  }

  zip.generateAsync({ type: 'blob' }).then((content) => {
    const a = document.createElement('a');
    a.href = URL.createObjectURL(content);
    a.download = 'output.zip';
    a.click();
  });
});
