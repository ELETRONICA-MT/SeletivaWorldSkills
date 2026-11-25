async function submissionAsset(action,id,mod,part,body){
 const {data:{session}}=await cloud.auth.getSession();
 if(!session)throw Error("Entre novamente");
 const r=await fetch("https://iogcgzzkeflicycmwyzc.supabase.co/functions/v1/ws16-submissions",{method:"POST",headers:{Authorization:"Bearer "+session.access_token,"x-ws-action":action,"x-ws-id":id,"x-ws-module":mod,"x-ws-part":String(part),"Content-Type":"application/octet-stream"},body});
 if(!r.ok){const j=await r.json().catch(()=>({}));throw Error((j.error||"Falha GitHub")+" "+(j.status||r.status))}
 return action==="download"?r.blob():r.json();
}
async function uploadSubmission(file,mod,group,session){
 const id=crypto.randomUUID(),original=file.webkitRelativePath||file.name,path=session.user.id+"/"+mod+"/github/"+group+"/"+id,parts=Math.ceil(file.size/4000000);
 if(file.size<1||file.size>50000000)throw Error("Arquivo fora do limite de 50 MB");
 try{for(let i=0;i<parts;i++)await submissionAsset("upload",id,mod,i,file.slice(i*4000000,(i+1)*4000000));
 const m=await cloud.from("ws16_submissions").insert({id,user_id:session.user.id,user_name:String(session.user.user_metadata?.name||"Competidor").slice(0,120),module:mod,filename:original.slice(0,200),storage_path:path,bytes:file.size});if(m.error)throw m.error;
 }catch(e){for(let i=0;i<parts;i++){try{await submissionAsset("delete",id,mod,i)}catch{}}throw e}
}
async function downloadSubmission(d){
 if(!d.storage_path.includes("/github/")){const r=await cloud.storage.from("ws16-submissions").download(d.storage_path);if(r.error)throw r.error;return r.data}
 const chunks=[];for(let i=0;i<Math.ceil(d.bytes/4000000);i++)chunks.push(await submissionAsset("download",d.id,d.module,i));
 const blob=new Blob(chunks);if(blob.size!==Number(d.bytes))throw Error("Tamanho incorreto");return blob;
}
async function deleteSubmission(d){
 if(!d.storage_path.includes("/github/")){const r=await cloud.storage.from("ws16-submissions").remove([d.storage_path]);if(r.error)throw r.error;return}
 for(let i=0;i<Math.ceil(d.bytes/4000000);i++)await submissionAsset("delete",d.id,d.module,i);
}
