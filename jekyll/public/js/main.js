$(function() {
  // Viewer.js
  const viewer = new Viewer($("img.viewer"), {
    title: false,
    tooltip: false,
    movable: false,
    zoomable: false,
    rotatable: false,
    scalable: false,
    fullscreen: false,
    toolbar: false
  });
});
