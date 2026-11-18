# WS16 Engineering Hub — versão inicial

Portal estático para o GitHub Pages. Os arquivos originais da pasta `Simulados/` permanecem intactos.

## Publicação
Após aprovar e mesclar o pull request, abra **Settings → Pages** no repositório e escolha **Deploy from a branch**, branch **main**, pasta **/docs**. O endereço esperado é:
https://eletronica-mt.github.io/SeletivaWorldSkills/

## Como cadastrar projetos
Edite `docs/projects.json`. Cada item em `projects` possui:
- `id`: identificador único
- `module`: `A`, `B` ou `C`
- `title` e `description`
- `repositoryPath`: pasta existente dentro do repositório, por exemplo `Simulados/CX_TECH`
- `model`: caminho relativo ao `docs/index.html`, por exemplo `assets/modelos/bomb-a.glb`, ou `null`
- `photos`: lista de caminhos relativos, por exemplo `["assets/fotos/bomb-a/01.jpg"]`
- `documents`: lista de objetos `{"title":"Enunciado","path":"assets/documentos/bomb-a.pdf"}`

A classificação de CX_TECH como A e IO_CAR como B é apenas **provisória**; corrija-a antes da publicação conforme a organização real dos simulados.

## Contador da competição
Preencha `competition.date` com data, hora e fuso explícito, por exemplo `"2027-09-10T09:00:00-03:00"`. Enquanto não houver data oficial, o site mostra traços.

## Organização sugerida
```
docs/
  index.html
  projects.json
  assets/
    modelos/
    fotos/
    documentos/
Simulados/        # permanece sob o fluxo atual de entregas do aluno
```

## Limitações e cuidados
- GitHub Pages é público e estático; não oferece upload autenticado diretamente no portal.
- Para adicionar arquivos, faça commit pelo GitHub ou pelo fluxo de trabalho já existente.
- O visualizador 3D carrega arquivos GLB/GLTF com o componente model-viewer, via CDN. Modelos muito grandes podem falhar ou ficar lentos.
- O portal não altera nem avalia entregas do aluno. Não publique gabaritos, avaliações confidenciais ou informações pessoais.
- A galeria e a biblioteca são alimentadas pelo JSON; o portal não faz varredura automática das pastas do GitHub.
