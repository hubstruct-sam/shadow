#include "sdw.h"
#include "utils.h"

i32 main()
{
  sdw_t sdw{};

  // Temp way to do it, might have multiple pipilines to handle
  // unless subpasses are create the complete shader chain.
  sdw.pip.shaders.spirv.push_back(ut_read_bin("build/vertex.spv"));
  sdw.pip.shaders.spirv.push_back(ut_read_bin("build/fragment.spv"));
  sdw.pip.shaders.types.push_back(VK_SHADER_STAGE_VERTEX_BIT);
  sdw.pip.shaders.types.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);

  sdw_ini(&sdw, 800u, 600u, "Shadow");
  sdw_run(&sdw);
  sdw_del(&sdw);

  return 0;
}