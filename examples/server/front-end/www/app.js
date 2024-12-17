document.getElementById('resizeForm').addEventListener('submit', function(event) {
    event.preventDefault();

    const formData = new FormData();
    const fileInput = document.getElementById('image');
    const widthInput = document.getElementById('width');
    const heightInput = document.getElementById('height');

    formData.append('image', fileInput.files[0]);
    formData.append('width', widthInput.value);
    formData.append('height', heightInput.value);

    // Verzenden van het formulier naar de server
    fetch('http://localhost:3002/resize', {
      method: 'POST',
      body: formData
    })
    .then(response => response.blob()) // Ontvang de afbeelding als blob
    .then(blob => {
      const url = URL.createObjectURL(blob);
      document.getElementById('resizedImage').src = url; // Toon de resized afbeelding
    })
    .catch(error => {
      console.error('Er is een fout opgetreden:', error);
    });
  });