let imageBlobs = [], audioBlobs = [];
let typingTimeouts = {}; // To store timeouts for each textarea

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

  // Check if all files are present after processing
  checkFilesForDownload();
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

  return new Promise((resolve) => canvas.toBlob(resolve, 'image/png'));
}

async function generateSpeech(text) {
  const response = await fetch('http://tts-engine:3000/tts', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ text: text, language: 'nl' })
  });

  if (!response.ok) throw new Error('Text-to-Speech API error');

  return await response.blob();
}

document.addEventListener('DOMContentLoaded', () => {
  for (let i = 1; i <= 8; i++) {
    const imageInput = document.getElementById(`image${i}`);
    const resizedImageId = `resizedImage${i}`;

    imageInput.addEventListener('change', (event) => 
      handleImageSelection(event, resizedImageId, i)
    );

    const textArea = document.getElementById(`textInput${i}`);
    textArea.addEventListener('input', () => handleTyping(textArea, i));
  }
});

async function handleImageSelection(event, resizedImageId, index) {
  const fileInput = event.target;
  const imageFile = fileInput.files[0];
  if (!imageFile) return;

  const formData = new FormData();
  formData.append('image', imageFile);
  formData.append('width', 240);
  formData.append('height', 240);

  try {
    const imageResponse = await fetch('http://cropper-engine:3000/resize', {
      method: 'POST',
      body: formData
    });

    if (!imageResponse.ok) throw new Error('Failed to resize image');

    const imageBlob = await imageResponse.blob();
    const imageUrl = URL.createObjectURL(imageBlob);

    document.getElementById(resizedImageId).src = imageUrl;

    const img = new Image();
    img.src = imageUrl;
    img.onload = () => {
      const canvas = document.createElement('canvas');
      const ctx = canvas.getContext('2d');
      canvas.width = 240;
      canvas.height = 240;
      ctx.drawImage(img, 0, 0, 240, 240);

      canvas.toBlob((blob) => {
        imageBlobs[index] = blob;
      }, 'image/png');
    };
  } catch (error) {
    console.error('Error processing image:', error);
  }

  checkFilesForDownload();
}

function handleTyping(textArea, index) {
  if (typingTimeouts[index]) {
    clearTimeout(typingTimeouts[index]);
  }

  typingTimeouts[index] = setTimeout(async () => {
    const text = textArea.value;
    if (text) {
      const audioBlob = await generateSpeech(text);
      document.getElementById(`audioPlayer${index}`).src = URL.createObjectURL(audioBlob);
      audioBlobs[index] = audioBlob;
    }

    checkFilesForDownload();
  }, 1000);
}

function checkFilesForDownload() {
  const downloadButton = document.getElementById('downloadZip');
  let allFilesPresent = true;

  for (let i = 1; i <= 8; i++) {
    if (!imageBlobs[i] || !audioBlobs[i]) {
      allFilesPresent = false;
      break;
    }
  }

  downloadButton.disabled = !allFilesPresent;
}

document.getElementById('downloadZip').addEventListener('click', () => {
  const zip = new JSZip();

  for (let i = 1; i <= 8; i++) {
    if (imageBlobs[i]) {
      zip.file(`image${i}.png`, imageBlobs[i]);
    }
    if (audioBlobs[i]) {
      zip.file(`audio${i}.wav`, audioBlobs[i]);
    }
  }

  zip.generateAsync({ type: 'blob' }).then((content) => {
    const a = document.createElement('a');
    a.href = URL.createObjectURL(content);
    a.download = 'output.zip';
    a.click();
  });
});
