#include "sdw.h"
#include "utils.h"

i32 main()
{
  sdw_t sdw{};

  // Temp way to do it, might be multiple pipilines to handle
  // unless subpasses are sufficient to concatenate shaders.
  sdw.pip.shaders.spirv.push_back(ut_read_bin("build/vertex.spv"));
  sdw.pip.shaders.spirv.push_back(ut_read_bin("build/fragment.spv"));
  sdw.pip.shaders.types.push_back(VK_SHADER_STAGE_VERTEX_BIT);
  sdw.pip.shaders.types.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);

  sdw_ini(&sdw, 500u, 500u, "Shadow");
  sdw_run(&sdw);
  sdw_del(&sdw);

  return 0;
}