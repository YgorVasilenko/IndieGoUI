# UI editor
Simple editor. Refer to [top-level readme](../README.md) for instructions to build and run.

## PROJECT_DIR
Editor reads ```$PROJECT_DIR``` env var on startup. If it's not available, dir where execurtable resides will be used on startup.<br>
After editor start, ```PROJECT_DIR``` can be easily changed from editor. When created UI is saved, paths to all materials (fonts and images) saved either:<br>

1. Relative to ```PROJECT_DIR```, if they reside under PROJECT_DIR tree
2. As absolute path, if material is outside of ```PROJECT_DIR```

When UI is loaded in application, one should provide ```Manager::project_dir```, so IndieGoUI would know, where to load stuff from!