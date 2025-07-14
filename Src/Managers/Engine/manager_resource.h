// Managers/manager_resourse.h
#pragma once

#include <string>

#include "manager_base.h"

namespace EDD::Managers {
enum class ResourceType {
  NONE = 0,
  TEXTURE = 1,
  AUDIO = 2,
  FONT = 3,
  MODEL = 4,
};

/*
Менеджер ресурсов отвечает
      за загрузку/выгрузку ресурсов в движке.
      за объединение ресурсов в группы (entity+texture, entity+audio и т.д.) 
      разграничение доступа к ресурсам (холодные и горячие ресурсы) 
      поиск ресурсов по типу (texture, audio, font, model и т.д.)
      поиск ресурсов по тегу/имени (например, найти все текстуры с тегом "player")
      передачу ресурсов менеджеру рендера

*/
class Resource : public Managers::Base {
 public:
 private:
  std::string resourse_path_;

 public:
  Resource() {}
  ~Resource() {}
  virtual void Update() override {}

 private:
  bool LoadUnit(const std::string& path, ResourceType resourese_type) {
    return true;
  }

  bool LoadFolder(const std::string& path, const ResourceType resourese_type) {
    return true;
  }

  virtual void Init(std::initializer_list<void*> args = {}) override {}
  virtual void FreeResources() override {}
};
}  // namespace EDD::Managers