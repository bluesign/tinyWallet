import MetadataViews from 0x1d7e57aa55817448
import NonFungibleToken from 0x1d7e57aa55817448

pub fun main(ownerAddress: Address, path:String) : [{String : AnyStruct}] {

    var items : [{String : AnyStruct}] = []

    let account = getAuthAccount(ownerAddress)
    var sr = account.borrow<&NonFungibleToken.Collection>(from: StoragePath(identifier:path)!)!

    for id in sr.getIDs(){
      var item :{String:AnyStruct} = {}
      var nft = sr.borrowNFT(id: id)
      item["id"] = id.toString()
      if let display = nft.resolveView(Type<MetadataViews.Display>()){
        item["name"] = (display as! MetadataViews.Display).name
        item["thumbnail"] = (display as! MetadataViews.Display).thumbnail.uri()
      }
      else{
          item["name"] = id.toString()
      }
      items.append(item)
    }

    return items
}